#include "tslib_test.h";
#include "../../orcadefaults.h"

void test_tsstring();
void test_tsarray();
void test_tslist();

void dumpmem() {
        //_tslog->info("Mem Available : %x\n",(int)_tsmem->available());
}

int main(int argc, char** argv)
{
    tslib_init();
    while(1) {
        test_tsstring();
        test_tslist();
        test_tsarray();        
    }
}

void test_tsstring() {
        tsstring_t* v1 = _tsstring->new_c("this is a test of the emergency broadcasting system");
        _tslog->info("Created string [%s]\n", v1->data);

        tslist_t* vl = _tsstring->split(v1, "emer\0");
        _tslog->info("Did split\n");
        
        for(int scount = 0; scount < vl->count; scount++) {
                tsstring_t* seg = _tslist->get(vl, scount);
                _tslog->info("Segment #%i is [%s]\n",scount, seg->data);
                _tsstring->free(&seg);
        };
        //vl->free(vl);

        tsstring_t* v2 = _tsstring->clone(v1);
        _tslog->info("Cloned string [%s]\n", v2->data);


        _tsmem->fill(v2->data, 'A', 10);
        _tslog->info("Filled 10 characters with 'A' [%s]\n", v2->data);


        _tsstring->clear(v2);
        _tslog->info("Cleared string [%s]\n", v2->data);


        tsstring_t* v3 = _tsstring->substring(v1, 7, 9);
        _tslog->info("Substring is [%s]\n", v3->data);


        tslib_size_t idx = _tsstring->indexof(v1,"emergency");
        _tslog->info("Index of = %i\n", idx);

        tsstring_t* v4 = _tsstring->substring(v1, idx, 4);
        _tslog->info("Sub of index = %s\n", v4->data);


        _tslog->info("v1 = %s\n", v1->data);
        _tslog->info("v2 = %s\n", v2->data);
        _tslog->info("v3 = %s\n", v3->data);
        _tslog->info("v4 = %s\n", v4->data);

        _tsstring->free(&v1);
        _tsstring->free(&v2);
        _tsstring->free(&v3);
        _tsstring->free(&v4);

        _tslog->info("done with strings\n");

}


void test_tslist() {

        tslist_t* list = _tslist->new(10);

        _tslist->add(list, "blah blah should be 1");
        _tslist->add(list,"this will be item 2");
        _tslist->insertAt(list,0,"inserted at item 0");
        _tslist->insertAt(list,2,"this should be item 2");
        _tslist->removeAt(list,2);

        for(int index=0; index < list->count; index++) {
                void* item = _tslist->get(list,index);
                if (item) {
                        _tslog->info("%u : %s\n",index,item);
                }
        }

}

void test_tsarray() {
        tsarray_t* arr = _tsarray->new(100);

        _tslog->info("inserting 0\n");
        _tsarray->set(arr,0,"This is a test");

        _tslog->info("inserting 1\n");
        _tsarray->set(arr,20,"This is another test");

        _tsarray->resize(arr,2);

        _tslog->info("Starting iteration\n");
        for(int index=0; index < arr->count; index++) {
                void* item = _tsarray->get(arr,index);
                if (item) {
                        _tslog->info("%u : %s\n",index,item);
                }
        }

}