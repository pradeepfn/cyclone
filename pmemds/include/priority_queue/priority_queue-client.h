#ifndef PMEMDS_PRIORITY_QUEUE_CLIENT_H
#define PMEMDS_PRIORITY_QUEUE_CLIENT_H


#include "../pmemds-client.h"

namespace pmemdsclient{

    class PriorityQueueEngine:public PMEngine{


    public:
        PriorityQueueEngine(PMClient *handle, const uint16_t ds_id, size_t size, unsigned long core_mask);
        ~PriorityQueueEngine();

        int create(uint8_t flags);
        int close();
        int remove();

        int insert(unsigned long key, unsigned long priority);

        int topk();

        int erase(unsigned long key);

        int increase_prio(const unsigned &key, unsigned long &delta_prio);

        int decrease_prio(const unsigned &key, unsigned long &delta_prio);

    private:
        std::string ds_name;
    };


}


#endif //PMEMDS_PRIORITY_QUEUE_CLIENT_H
