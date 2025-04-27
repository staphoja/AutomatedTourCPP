#ifndef HASHMAP_H
#define HASHMAP_H

#include <functional>
#include <list>
#include <string>
#include <iostream>

template <class T>
class HashMap {
public:
	// constructor 
	HashMap(double max_load = 0.75) : m_nKeys(0), m_nBuckets(10), m_maxLoad(max_load) {
		m_buckets = new std::list<std::pair<std::string, T>>[m_nBuckets];
	}

	// destructor; deletes all of the items in the hashmap 
	~HashMap() {
		delete[] m_buckets;
	}

	// return the number of associations in the hashmap 
	int size() const {
		return m_nKeys;
	}

	// The insert method associates one item (key) with another (value). 
	// If no association currently exists with that key, this method inserts 
	// a new association into the hashmap with that key/value pair. If there is 
	// already an association with that key in the hashmap, then the item 
	// associated with that key is replaced by the second parameter (value). 
	// Thus, the hashmap must contain no duplicate keys. 
	void insert(const std::string& key, const T& value) {

		rehash(m_nKeys + 1);

		if (find(key) == nullptr) {
			m_buckets[mapFunction(key)].push_back(std::pair<std::string, T>(key, value));
			m_nKeys++;
		}

		return;
	}

	// Defines the bracket operator for HashMap, so you can use your map like this: 
	// your_map["david"] = 2.99; 
	// If the key does not exist in the hashmap, this will create a new entry in 
	// the hashmap and map it to the default value of type T (0 for builtin types). 
	// It returns a reference to the newly created value in the map. 
	T& operator[](const std::string& key) {

		T* value = find(key);
		if (value == nullptr) {
			insert(key, *value);
			return *find(key);
		}
		return *find(key);
	}

	// If no association exists with the given key, return nullptr; otherwise, 
	// return a pointer to the value associated with that key. This pointer can be 
	// used to examine that value within the map. 
	const T* find(const std::string& key) const {
		typename std::list<std::pair<std::string, T>>::const_iterator it;
		for (it = m_buckets[mapFunction(key)].begin();
			it != m_buckets[mapFunction(key)].end(); it++) {
			if ((*it).first == key)
				return &(*it).second;
		}
		return nullptr;
	}

	// If no association exists with the given key, return nullptr; otherwise, 
	// return a pointer to the value associated with that key. This pointer can be 
	// used to examine that value or modify it directly within the map. 
	T* find(const std::string& key) {
		const auto& hm = *this;
		return const_cast<T*>(hm.find(key));
	}

private:
	const int mapFunction(const std::string hashMe) const {
		std::hash<std::string> str_hash;
		int hashValue = str_hash(hashMe);

		int bucketNum = hashValue % m_nBuckets;
		return bucketNum;
	}

	bool rehash(unsigned int numKeys) {
		if (getLoadFactor(numKeys) > m_maxLoad) {

			m_nBuckets *= 2;
			std::list<std::pair<std::string, T>>* tempBuckets = new std::list<std::pair<std::string, T>>[m_nBuckets];

			for (int i = 0; i < m_nBuckets / 2; i++) {
				for (typename std::list<std::pair<std::string, T>>::iterator it = m_buckets[i].begin(); it != m_buckets[i].end(); it++) {
					std::string key = (*it).first;
					T value = (*it).second;

					tempBuckets[mapFunction(key)].push_back(std::pair<std::string, T>(key, value));
				}
			}

			delete[] m_buckets;

			m_buckets = new std::list<std::pair<std::string, T>>[m_nBuckets];

			for (int i = 0; i < m_nBuckets; i++) {
				for (typename std::list<std::pair<std::string, T>>::iterator it = tempBuckets[i].begin(); it != tempBuckets[i].end(); it++) {
					std::string key = (*it).first;
					T value = (*it).second;

					m_buckets[mapFunction(key)].push_back(std::pair<std::string, T>(key, value));
				}
				
			}

			delete[] tempBuckets;

			return true;
		}
		else {
			return false;
		}
	}

	double getLoadFactor(int numKeys) {
		return (numKeys / (double)m_nBuckets);
	}

	HashMap(const HashMap&) = delete;
	HashMap& operator=(const HashMap&) = delete;

	unsigned int m_nKeys;
	unsigned int m_nBuckets;
	double m_maxLoad;
	std::list<std::pair<std::string, T>>* m_buckets;
};

#endif // HASHMAP_H