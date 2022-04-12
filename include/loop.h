#ifndef LOOP_H
#define LOOP_H

#include "pin.H"

#include <cstdlib>
#include <cstring>
#include <map>
#include <list>
#include <set>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <cmath>
#include <cstdio>
#include <stack>

using std::iostream;
using std::ofstream;
using std::string;
using std::endl;
using std::cout;
using std::cerr;
using std::hex;
using std::ios;
using std::unordered_map;
using std::set;
using std::list;

struct BasicBlock{
    uint32_t head;
    uint32_t sp;
    uint32_t count;
    uint32_t last_tid;
    uint32_t next_bbhead;
};

class Loop{
    unordered_map<uint32_t, BasicBlock> addr2bb;
    uint32_t* bbtrace = NULL;
    uint32_t current_traceid = 0;
    uint32_t bbtrace_len = 0;
    // unordered_map<uint32_t, LoopInfo> addr2bb;
    public:
        void process_bb(uint32_t bbhead, uint32_t insnum, uint32_t sp, uint32_t nexthead);
        void clear_stack(uint32_t sp);
        uint32_t insert_bb2trace(uint32_t bbhead);
        void clean_aloop(uint32_t start_tid, uint32_t end_tid);
        void fini();
};

#endif //DFGPLUS_ENCRYPTION_HANDLER_H