#include "tslib_test.h";
#include "../../orcadefaults.h"

void test_tsstring();
void test_tsarray();
void test_tslist();

tsmem_vtbl_t*    _tsmem;
tsstring_vtbl_t* _tsstring;

void dumpmem() {
        //printf("Mem Available : %x\n",(int)_tsmem->available());
}

int main(int argc, char** argv)
{
   _tsmem = tsmem_init();
   _tsstring = tsstring_init();

        while(1) {
   test_tsstring();
   test_tslist();
   test_tsarray();        

        }
}

void test_tsstring() {
        tsstring_t* v1 = _tsstring->create_c("this is a test of the emergency broadcasting system");
        printf("Created string [%s]\n", v1->data);

        tslist_t* vl = v1->f->split(v1, "emer\0");
        
        for(int scount = 0; scount < vl->count; scount++) {
                tsstring_t* seg = vl->get(vl, scount);
                printf("Segment #%d is [%s]\n",scount, seg->data);
                seg->f->free(&seg);
        };
        //vl->free(vl);

        tsstring_t* v2 = v1->f->clone(v1);
        printf("Cloned string [%s]\n", v2->data);


        _tsmem->fill(v2->data, 'A', 10);
        printf("Filled 10 characters with 'A' [%s]\n", v2->data);


        v2->f->clear(v2);
        printf("Cleared string [%s]\n", v2->data);


        tsstring_t* v3 = v1->f->substring(v1, 7, 9);
        printf("Substring is [%s]\n", v3->data);


        tslib_size_t idx = v1->f->indexof(v1,"emergency");
        printf("Index of = %d\n", idx);

        tsstring_t* v4 = v1->f->substring(v1, idx, 4);
        printf("Sub of index = %s\n", v4->data);


        printf("v1 = %s\n", v1->data);
        printf("v2 = %s\n", v2->data);
        printf("v3 = %s\n", v3->data);
        printf("v4 = %s\n", v4->data);

        v1->f->free(&v1);
        v2->f->free(&v2);
        v3->f->free(&v3);
        v4->f->free(&v4);

        printf("done with strings\n");

}


void test_tslist() {

        tslist_t* list = tslist_create(10);

        list->add(list, "blah blah should be 1");
        list->add(list,"this will be item 2");
        list->insertAt(list,0,"inserted at item 0");
        list->insertAt(list,2,"this should be item 2");

        list->removeAt(list,2);

        for(int index=0; index < list->count; index++) {
                void* item = list->get(list,index);
                if (item) {
                        printf("%u : %s\n",index,item);
                }
        }

}

void test_tsarray() {
        tsarray_t* arr = tsarray_create(100);

        printf("inserting 0\n");
        arr->set(arr,0,"This is a test");

        printf("inserting 1\n");
        arr->set(arr,20,"This is another test");

        arr->resize(arr,2);

        printf("Starting iteration\n");
        for(int index=0; index < arr->count; index++) {
                void* item = arr->get(arr,index);
                if (item) {
                        printf("%u : %s\n",index,item);
                }
        }

}