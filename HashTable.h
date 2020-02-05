#ifndef __HASHTABLE__
#define __HASHTABLE__

#include "HashUtils.h"

// Do not modify the public interface of this class.
// Otherwise, your code will note compile!
template <class T>
class HashTable {
    struct Entry {
        std::string Key;             // the key of the entry
        T Value;   // the value of the entry
        bool Deleted;        // flag indicating whether this entry is deleted
        bool Active;         // flag indicating whether this item is currently used

        Entry() : Key(), Value(), Deleted(false), Active(false) {}
    };

    struct Bucket {
        Entry entries[3];
    };

    int _capacity; // INDICATES THE SIZE OF THE TABLE
    int _size; // INDICATES THE NUMBER OF ITEMS IN THE TABLE

    Bucket* _table; // HASH TABLE
    int totalprobe;
    int unsucprob;
    
    // You can define private methods and variables

public:
    // TODO: IMPLEMENT THESE FUNCTIONS.
    // CONSTRUCTORS, ASSIGNMENT OPERATOR, AND THE DESTRUCTOR
    HashTable();
    HashTable(const HashTable<T>& rhs);
    HashTable<T>& operator=(const HashTable<T>& rhs);
    ~HashTable();

    // TODO: IMPLEMENT THIS FUNCTION.
    // INSERT THE ENTRY IN THE HASH TABLE WITH THE GIVEN KEY & VALUE
    // IF THE GIVEN KEY ALREADY EXISTS, THE NEW VALUE OVERWRITES
    // THE ALREADY EXISTING ONE. 
    // IF LOAD FACTOR OF THE TABLE IS BIGGER THAN 0.5,
    // RESIZE THE TABLE WITH THE NEXT PRIME NUMBER.
    void Insert(std::string key, const T& value);

    // TODO: IMPLEMENT THIS FUNCTION.
    // DELETE THE ENTRY WITH THE GIVEN KEY FROM THE TABLE
    // IF THE GIVEN KEY DOES NOT EXIST IN THE TABLE, JUST RETURN FROM THE FUNCTION
    // HINT: YOU SHOULD UPDATE ACTIVE & DELETED FIELDS OF THE DELETED ENTRY.
    void Delete(std::string key);

    // TODO: IMPLEMENT THIS FUNCTION.
    // IT SHOULD RETURN THE VALUE THAT CORRESPONDS TO THE GIVEN KEY.
    // IF THE KEY DOES NOT EXIST, THIS FUNCTION MUST RETURN T()
    T Get(std::string key) const;

    // TODO: IMPLEMENT THIS FUNCTION.
    // AFTER THIS FUNCTION IS EXECUTED THE TABLE CAPACITY MUST BE
    // EQUAL TO newCapacity AND ALL THE EXISTING ITEMS MUST BE REHASHED
    // ACCORDING TO THIS NEW CAPACITY.
    // WHEN CHANGING THE SIZE, YOU MUST REHASH ALL OF THE ENTRIES FROM 0TH ENTRY TO LAST ENTRY
    void Resize(int newCapacity);
    
    // TODO: IMPLEMENT THIS FUNCTION.
    // RETURNS THE AVERAGE NUMBER OF PROBES FOR SUCCESSFUL SEARCH
    double getAvgSuccessfulProbe();
    
    // TODO: IMPLEMENT THIS FUNCTION.
    // RETURNS THE AVERAGE NUMBER OF PROBES FOR UNSUCCESSFUL SEARCH
    double getAvgUnsuccessfulProbe();

    // THE IMPLEMENTATION OF THESE FUNCTIONS ARE GIVEN TO YOU
    // DO NOT MODIFY!
    int Capacity() const;
    int Size() const;
};



template <class T>
HashTable<T>::HashTable() {
    _table = NULL;
    _capacity = 0;
    _size = 0;
    totalprobe = 0;
    unsucprob = 0;
}

template <class T>
HashTable<T>::HashTable(const HashTable<T>& rhs) {
    _table = NULL;
    _size = 0; 
    unsucprob = rhs.unsucprob;
    totalprobe = rhs.totalprobe;
    _capacity = rhs._capacity;
    _table = new Bucket[_capacity];
    Bucket* rhstable = rhs._table;
    for(int i=0 ; i<_capacity ; i++){
        for(int k=0 ; k<3 ; k++){
            if(rhstable[i].entries[k].Active){
                Insert(rhstable[i].entries[k].Key, rhstable[i].entries[k].Value);
            }
        }
    }
}

template <class T>
HashTable<T>& HashTable<T>::operator=(const HashTable<T>& rhs) {
    if(_table) delete [] _table;
    _table = NULL;
    _size = 0;
    unsucprob = rhs.unsucprob;
    totalprobe = rhs.totalprobe;
    _capacity = rhs._capacity;
    _table = new Bucket[_capacity];   
    for(int i=0 ; i< _capacity ; i++){
        for(int k=0 ; k<3 ; k++){
            if(rhs._table[i].entries[k].Active){
                Insert(rhs._table[i].entries[k].Key, rhs._table[i].entries[k].Value);
            }
        }
    }
    return *this;
}

template <class T>
HashTable<T>::~HashTable() {
    if(_table) delete [] _table;
    _table = NULL;
    _capacity = 0;
    _size = 0;
    totalprobe = 0;
    unsucprob = 0;
}

template <class T>
void HashTable<T>::Insert(std::string key, const T& value) {
    double lf;

    if(_table==NULL && _capacity==0){
        int newcapacity = NextCapacity(0);
        _table = new Bucket[newcapacity];
        _capacity = newcapacity;
    }

    lf = (double) _size/(_capacity*3);
    if(lf>=0.5) Resize(NextCapacity(_capacity));
    int quad = 1;
    int bucket = Hash(key) % _capacity;
    for(; bucket<_capacity ; ){
        for(int j=0 ; j<3 ; j++){
            unsucprob++;
            if(!_table[bucket].entries[j].Active){
                _size = _size +1;
                _table[bucket].entries[j].Active = true;
                _table[bucket].entries[j].Key = key;
                _table[bucket].entries[j].Value = value;
                totalprobe++;
                unsucprob++;
                return;
            }
            else if(_table[bucket].entries[j].Active && _table[bucket].entries[j].Key == key){
                _table[bucket].entries[j].Key = key;
                _table[bucket].entries[j].Value = value;
                totalprobe++;
                unsucprob++;
                return;
            }
        }
        unsucprob++;
        bucket = (quad*quad + bucket) % _capacity;
        quad++;
        totalprobe++;
    }

}

template <class T>
void HashTable<T>::Delete(std::string key) {
    if(_table){
        int bucket = Hash(key) % _capacity;
        int quad = 1;
        while(true){ 
            for(int j=0 ; j<3 ; j++){
                if(_table[bucket].entries[j].Active && _table[bucket].entries[j].Key==key){
                    _table[bucket].entries[j].Active = false;
                    _table[bucket].entries[j].Deleted = true;
                    _size--;
                    return;
                }
                else if(!_table[bucket].entries[j].Active && !_table[bucket].entries[j].Deleted){
                    return;
                }
            }
            bucket = (bucket + quad*quad) % _capacity;
            quad++;
            if(bucket == Hash(key) % _capacity) return;
        }
    }
}

template <class T>
T HashTable<T>::Get(std::string key) const {
    if(_table==NULL) return T();
    int bucket = Hash(key) % _capacity;
    int quad = 1;

    while(true){
        for(int i=0 ; i<3 ; i++){
            if(_table[bucket].entries[i].Active && _table[bucket].entries[i].Key == key){
                return _table[bucket].entries[i].Value;
            }

            else if(!_table[bucket].entries[i].Active && !_table[bucket].entries[i].Deleted){
                return T();
            }
        }
        bucket = (bucket + quad*quad) % _capacity;
        quad++;
        if(bucket == Hash(key) % _capacity) return T();
    }
    return T();
}

template <class T>
void HashTable<T>::Resize(int newCapacity) {
    HashTable newhashtable;
    int newcap = NextCapacity(this->_capacity);
    newhashtable._table = new Bucket[newcap];
    newhashtable._capacity = newcap;
    for(int i=0 ; i< _capacity ; i++){
        for(int j=0 ; j<3 ; j++){
            if(_table[i].entries[j].Active && !_table[i].entries[j].Deleted){
                newhashtable.Insert( _table[i].entries[j].Key , _table[i].entries[j].Value );
            }
        }
    }
    *this = newhashtable;
    newhashtable.~HashTable();
}


template <class T>
double HashTable<T>::getAvgSuccessfulProbe() {
    return ((double)totalprobe / _size);

}

template <class T>
double HashTable<T>::getAvgUnsuccessfulProbe() {
    return ((double) unsucprob/(_capacity*3));

}

template <class T>
int HashTable<T>::Capacity() const {
    return _capacity;
}

template <class T>
int HashTable<T>::Size() const {
    return _size;
}

#endif