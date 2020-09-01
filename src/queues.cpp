#include "queues.hpp"

template class QueT<uint8_t,2048>;

template <typename T,uint32_t size>
QueT<T,size>::QueT()
{pThis = this;} 

template <typename T,uint32_t size>
void QueT<T,size>::push(T x)
{
    if(start==nullptr){start = &arr[count]; arr[count]=x;count++;}
    else
    {
        if(count<size)
        {
            for(uint32_t i=count;i>0;i--)
            {arr[i]=arr[i-1];}
            arr[0]=x;
            count++;
        }
    }
}

template <typename T, uint32_t size>
T QueT<T,size>::pop()
{
    T temp=0;
    if(count){count--;} //!< super kostyl'
    if(count)
    {
        temp=arr[count];
        arr[count]=0;
    }
    else
    {
            if (start!=nullptr)
            {
                temp=arr[0];arr[0]=0;
                start=nullptr;
            }
            else{/*cout<<"opa";*/}
    }
    return temp;
}