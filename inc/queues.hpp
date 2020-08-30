#ifndef QUEUES_HPP
#define QUEUES_HPP

#include "stdint.h"

/*! <Simple Static Queue With Const Max Size Must Be Checked On Empty Condition>*/
template <typename T, uint32_t size>
struct QueT
{
	QueT();
	void push(T x);    
    T pop();
    inline bool is_not_empty() //!< this condition must be checked before pop
    {
        //if(start==nullptr)return true;
        //        else return false;
        return (start) ? true : false;
    }    
    static QueT<T,size>* pThis;
private:
    T* start{nullptr}; //head of queue
    uint32_t count{0};
    //static constexpr uint32_t size=512; //size of queue
    T arr[size]{0};	
};
template <typename T,uint32_t size>
QueT<T,size>* QueT<T,size>::pThis = nullptr;

#endif //QUEUES_HPP