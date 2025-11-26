#ifdef __APPLE2GS__
segment "AUTOSEG~~~";
#endif

#include "../../orcadefaults.h"
#include "tsiplib_w5500_driver.h"
#include "w5500.h"
#include "w5500_defs.h"
#include "../../tslib/src/tslib.h"
#include "../../tsspilib/src/tsspilib.h"
#include "../../tsiplib/src/tsiplib.h"
#include "w5500_raw_socket.h"

#ifdef __MACOS__
#include "../../tsspilib_driver_ftdi/src/tsspilib_driver_ftdi.h"
#endif

#ifdef __APPLE2GS__
#include "../../tsspilib_driver_a2gpio/src/tsspilib_driver_a2gpio.h"
#endif

#define MAX_RING_FRAMES 8
static tsringbuf_t *frame_ring;
static uint8_t rx_buffer[ETH_MAX_FRAME_LEN];
static uint16_t rx_len = 0;

static void process_w5500_macraw_data_to_ring(const uint8_t *raw_data, uint16_t data_len);

static w5500_raw_socket_t* raw_socket;

bool tsiplib_w5500_driver_init(void)
{
        tslib_init();

        _tslog->info("Initializing SPI\n");
        tsspilib_device_vtbl_t* spi_device = NULL;

        #ifdef __APPLE2GS__
        spi_device = a2gpio_spi_driver_load();
        #else
        spi_device = ftdi_spi_driver_load();
        #endif

        if (spi_device==NULL) {
            printf("No device loaded\n");
            return SPI_ERROR;
        }
        spi_init(spi_device);

        _tslog->info("Initializing W5500\n");
        w5500_init();

        _tslog->info("Resetting W5500\n");
        w5500_reset();

        _tslog->info("Creating MACRAW Socket\n");
        raw_socket = w5500_raw_socket_create();

        /* Initialize the ring buffer */
        frame_ring = tsringbuf_create(MAX_RING_FRAMES, ETH_MAX_FRAME_LEN);
        if (!frame_ring) {
            _tslog->error("Failed to create frame ring buffer\n");
            return false;
        }

    return true;
}


static bool tsiplib_w5500_driver_send(const uint8_t *data, uint16_t len)
{
    printf("Sending %d bytes\n", len);
    raw_socket->send(raw_socket, data, len);
    return true;
}

static uint16_t tsiplib_w5500_driver_recv(uint8_t *buffer, uint16_t max_len)
{
    size_t frame_len;
    
    /* First, check if there are any unprocessed frames in the ring buffer */
    if (frame_ring->pop(frame_ring, buffer, max_len, &frame_len)) {
        printf("Returning buffered frame (%zu bytes)\n", frame_len);
        return (uint16_t)frame_len;
    }
    
    /* If ring buffer is empty, try to read new frames from W5500 */
    raw_socket->refresh(raw_socket);

    if (raw_socket->rx_bytes_available) {
        printf("Socket data available = %d bytes\r\n", raw_socket->rx_bytes_available);
        
        /* Read all available data into a temporary buffer */
        uint8_t temp_buffer[4096];  /* Larger buffer to hold multiple frames */
        uint16_t total_read = 0;
        
        /* Read data in chunks, respecting buffer size */
        while (raw_socket->rx_bytes_available > 0 && total_read < sizeof(temp_buffer)) {
            uint16_t to_read = raw_socket->rx_bytes_available;
            if (to_read > sizeof(temp_buffer) - total_read) {
                to_read = sizeof(temp_buffer) - total_read;
            }
            
            uint16_t bytes_read = raw_socket->receive(raw_socket, temp_buffer + total_read, to_read);
            if (bytes_read == 0) {
                break;
            }
            total_read += bytes_read;
            
            /* Refresh to update available byte count */
            raw_socket->refresh(raw_socket);
        }
        
        if (total_read > 0) {
            printf("Read %d bytes from W5500\n", total_read);
            
            /* Process the raw data to extract frames and populate ring buffer */
            process_w5500_macraw_data_to_ring(temp_buffer, total_read);
            
            /* Try to pop a frame from the ring buffer */
            if (frame_ring->pop(frame_ring, buffer, max_len, &frame_len)) {
                return (uint16_t)frame_len;
            }
        }
    }
    
    return 0;
}

void tsiplib_w5500_driver_get_mac_addr(eth_addr_t* addr) {
    w5500_get_SRCMAC(addr);
}
void tsiplib_w5500_driver_set_mac_addr(eth_addr_t* addr) {
    w5500_set_SRCMAC(addr);
}
void tsiplib_w5500_driver_get_ip_addr(ip_addr_t* addr) {
    w5500_get_SRCIP(addr);
}
void tsiplib_w5500_driver_set_ip_addr(ip_addr_t* addr) {
    w5500_set_SRCIP(addr);
}
void tsiplib_w5500_driver_get_subnet_mask(ip_addr_t* mask) {
    w5500_get_SUBMASK(mask);
}
void tsiplib_w5500_driver_set_subnet_mask(ip_addr_t* mask) {
    w5500_set_SUBMASK(mask);
}
void tsiplib_w5500_driver_get_gateway_addr(ip_addr_t* addr) {
    w5500_get_GWADDR(addr);
}
void tsiplib_w5500_driver_set_gateway_addr(ip_addr_t* addr) {
    w5500_set_GWADDR(addr);
}
void tsiplib_w5500_driver_dump_state() {
    w5500_dump_state();
}

static bool tsiplib_w5500_driver_get_link_status(void)
{
    w5500_phycfg_t phyconfig;
    w5500_get_PHYCFG(&phyconfig);
    if (phyconfig.link_status==W5500_PHYCFG_LINKSTATUS_UP)
        return true;
    return false;
}

static void tsiplib_w5500_driver_cleanup(void)
{
    if (frame_ring) {
        frame_ring->destroy(&frame_ring);
    }
}

net_driver_t* tsiplib_w5500_driver_create() {
    net_driver_t* rv = (net_driver_t*) malloc(sizeof(net_driver_t));
    rv->init = tsiplib_w5500_driver_init;
    rv->recv_packet = tsiplib_w5500_driver_recv;
    rv->send_packet = tsiplib_w5500_driver_send;
    rv->get_mac_addr = tsiplib_w5500_driver_get_mac_addr;
    rv->set_mac_addr = tsiplib_w5500_driver_set_mac_addr;
    rv->get_gateway_addr = tsiplib_w5500_driver_get_gateway_addr;
    rv->set_gateway_addr = tsiplib_w5500_driver_set_gateway_addr;
    rv->get_ip_addr = tsiplib_w5500_driver_get_ip_addr;
    rv->set_ip_addr = tsiplib_w5500_driver_set_ip_addr;
    rv->get_subnet_mask = tsiplib_w5500_driver_get_subnet_mask;
    rv->set_subnet_mask = tsiplib_w5500_driver_set_subnet_mask;
    rv->get_link_status = tsiplib_w5500_driver_get_link_status;
    rv->dump_status = tsiplib_w5500_driver_dump_state;
    return rv;
}

/* Internal version that populates the ring buffer */
static void process_w5500_macraw_data_to_ring(const uint8_t *raw_data, uint16_t data_len)
{
    uint16_t offset = 0;
    uint16_t frame_count = 0;
    uint16_t frames_added = 0;
    
    if (!raw_data || data_len == 0) {
        return;
    }
    
    /* Process frames and add them to ring buffer */
    while (offset < data_len && !frame_ring->is_full(frame_ring)) {
        /* Check if we have at least 2 bytes for the frame length */
        if (offset + 2 > data_len) {
            break;
        }
        
        /* W5500 prefixes each frame with a 2-byte length in big-endian format */
        uint16_t frame_len = (raw_data[offset] << 8) | raw_data[offset + 1];
        offset += 2;
        frame_len -= 2;
        
        /* Validate frame length */
        if (frame_len == 0 || frame_len > ETH_MAX_FRAME_LEN) {
            printf("Invalid frame length: %d\n", frame_len);
            break;
        }
        
        /* Check if we have enough data for the complete frame */
        if (offset + frame_len > data_len) {
            printf("Incomplete frame: need %d bytes but only %d available\n",
                   frame_len, data_len - offset);
            break;
        }
        
        /* Extract the Ethernet frame and add to ring buffer */
        const uint8_t *frame_data = &raw_data[offset];
        
        if (frame_ring->push(frame_ring, frame_data, frame_len)) {
            frames_added++;
            printf("Added frame %d to ring buffer (len=%d)\n", frames_added, frame_len);
        } else {
            printf("Ring buffer full, stopping frame processing\n");
            break;
        }
        
        /* Move to next frame */
        offset += frame_len;
        frame_count++;
        
        /* Check for padding between frames */
        if (offset < data_len && offset % 2 != 0) {
            offset++;
        }
    }
    
    printf("Processed %d frames, added %d to ring buffer\n", frame_count, frames_added);
}

void process_w5500_macraw_data(const uint8_t *raw_data, uint16_t data_len)
{
    uint16_t offset = 0;
    uint16_t frame_count = 0;
    
    if (!raw_data || data_len == 0) {
        printf("Invalid input: NULL data or zero length\n");
        return;
    }
    
    printf("\n=== Processing W5500 MACRAW data (%d bytes) ===\n", data_len);
    
    /* Process multiple frames in the buffer */
    while (offset < data_len) {
        /* Check if we have at least 2 bytes for the frame length */
        if (offset + 2 > data_len) {
            printf("Warning: Incomplete frame length header at offset %d\n", offset);
            break;
        }
        
        /* W5500 prefixes each frame with a 2-byte length in big-endian format */
        uint16_t frame_len = (raw_data[offset] << 8) | raw_data[offset + 1];
        offset += 2;
        
        printf("\n--- Frame %d ---\n", ++frame_count);
        printf("W5500 reported frame length: %d bytes\n", frame_len);
        
        /* Validate frame length */
        if (frame_len == 0) {
            printf("Error: Zero-length frame encountered\n");
            break;
        }
        
        if (frame_len > ETH_MAX_FRAME_LEN) {
            printf("Error: Frame length %d exceeds maximum Ethernet frame size (%d)\n", 
                   frame_len, ETH_MAX_FRAME_LEN);
            break;
        }
        
        /* Check if we have enough data for the complete frame */
        if (offset + frame_len > data_len) {
            printf("Error: Incomplete frame data. Expected %d bytes but only %d available\n",
                   frame_len, data_len - offset);
            break;
        }
        
        /* Extract and process the Ethernet frame */
        const uint8_t *frame_data = &raw_data[offset];
        
        /* Print the frame details using our existing function */
        eth_print_frame(frame_data, frame_len);
        
        /* Optionally, you could process the frame here */
        /* For example: pass it to your TCP/IP stack for processing */
        /* 
        if (net_driver && net_driver->recv_packet) {
            // Copy frame to receive buffer and process
            memcpy(rx_buffer, frame_data, frame_len);
            rx_len = frame_len;
            // Process would happen in main loop
        }
        */
        
        /* Move to next frame */
        offset += frame_len;
        
        /* Check for padding between frames (W5500 may align to 2-byte boundaries) */
        if (offset < data_len && offset % 2 != 0) {
            printf("Note: Skipping padding byte at offset %d\n", offset);
            offset++;
        }
    }
    
    printf("\n=== Processed %d frames, %d bytes total ===\n", frame_count, offset);
    
    if (offset < data_len) {
        printf("Warning: %d bytes remaining unprocessed\n", data_len - offset);
    }
}

