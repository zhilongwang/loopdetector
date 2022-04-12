#include "../include/loop.h"

#define TRACE_INC 10000
Loop::Loop(){
    bbtrace = (uint32_t *)malloc(TRACE_INC * sizeof(uint32_t));
    if (bbtrace == NULL){
        cout << "fail to allocate memory for bbtrace" << endl;
        exit(0);
    }
    bbtrace_len = TRACE_INC;
}

uint32_t Loop::insert_bb2trace(uint32_t bbhead) {
    if(current_traceid + 1 >= bbtrace_len){
        // cout << BLUE <<"[DEBUGING] "<< RESET <<" try to allocate memory for id2node_map_ (curr size:" << id2node_map_size << "allocate size:" <<  id2node_map_size + CHUNK_SIZE << ")" << endl;
        bbtrace_len += TRACE_INC;
        uint32_t * tmp =  bbtrace;
        cout << "trying to allocate memory" << ";";
        bbtrace = (uint32_t *)malloc(bbtrace_len * sizeof(uint32_t));
        if (bbtrace == NULL){
            cout << "fail to allocate memory" << endl;
            exit(0);
        }else{
            cout << "success to allocate memory" << ";";
        }
        memcpy(bbtrace, tmp, (current_traceid + 1) * sizeof(uint32_t));
        free(tmp);
    }
    bbtrace[++current_traceid] = bbhead;
    return current_traceid;
}
void Loop::clean_aloop(uint32_t start_tid, uint32_t end_tid){
    
}
void Loop::process_bb(uint32_t bbhead, uint32_t insnum, uint32_t sp, uint32_t nexthead){
    uint32_t tid =  insert_bb2trace(bbhead);
    auto it = addr2bb.find(bbhead);
    if(it != addr2bb.end()){
        if (sp == it->second.sp){
            clean_aloop(it->second.last_tid, tid);
        }else{
            ;
        }
        it->second.last_tid = tid;
    }else{
        BasicBlock bb = {bbhead, sp, 1, tid, nexthead};
        addr2bb[bbhead] = bb;
    }
}
void Loop::clear_stack(uint32_t sp){
    std::list<std::unordered_map<uint32_t, BasicBlock>::const_iterator> itrs;
 
    for (auto it = addr2bb.begin(); it != addr2bb.end(); it++)
    {
        if (it->second.sp >= sp){
            itrs.push_back(it);
        }
    }
    for (auto it: itrs) {
        addr2bb.erase(it);
    }
}

// addr2var_map_.erase(ad);