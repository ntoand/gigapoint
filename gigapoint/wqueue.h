#include <pthread.h>
#include <list>

//ref: http://vichargrave.com/multithreaded-work-queue-in-c/
 
using namespace std;

namespace gigapoint {
 
template <typename T> class wqueue
{

private:
    std::list<T>   m_queue;
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_condv;

public:
	wqueue() {
		pthread_mutex_init(&m_mutex, NULL);
		pthread_cond_init(&m_condv, NULL);
	}

  	~wqueue() {
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_condv);
	}

	void add(T item) {
	    pthread_mutex_lock(&m_mutex);
        typename std::list<T>::const_iterator iterator;
        for (iterator = m_queue.begin(); iterator != m_queue.end(); ++iterator) {
            if (*iterator == item)
            {
                int i =1 ;
            }
        }
	    m_queue.push_back(item);        
	    pthread_cond_signal(&m_condv);
	    pthread_mutex_unlock(&m_mutex);
	}

	T remove() {
	    pthread_mutex_lock(&m_mutex);
	    while (m_queue.size() == 0) {
	        pthread_cond_wait(&m_condv, &m_mutex);
	    }
	    T item = m_queue.front();
	    m_queue.pop_front();
	    pthread_mutex_unlock(&m_mutex);
	    return item;
	}

	int size() {
        pthread_mutex_lock(&m_mutex);
        int size = m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return size;
    }
};

}; //namespace gigapoint
