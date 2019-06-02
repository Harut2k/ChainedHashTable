#ifndef CHAINEDHASHTABLE_H
#define CHAINEDHASHTABLE_H
#include <list>



#define BASE_SIZE 100 // Base size of the chained hash map
#define MAX_CHAINED_DATA 5 //will resize the chained hash map by BASE_INCREASE times if the number of data chained for a certain key exceedes 5
#define BASE_INCREASE 2 // Represents by what factor the has map is resized by



template<typename TKey, typename TData>
class ChainedHashTable {
public:
	//element in hash table
	struct Entry {
		TKey Key;
		TData Data;
		Entry(TKey key, TData data) : Key(key), Data(data) {}
	};

public:
	// creates a hash table
	// must be given a hasher function, which can hash the key into an unsigned integer
	// size is the starting width of the hashmap, will change if the hashmap gets a chain longer than MAX_CHAINED_DATA
	ChainedHashTable(unsigned(*hasher)(const TKey &), unsigned size = BASE_SIZE);
	// copy an existing hashmap
	ChainedHashTable(const ChainedHashTable &map);

	~ChainedHashTable();

	// insert an entry into hashmap
	// if the hashmap has become overpopulated, will resize to increase searching efficency
	// true if inserted, false if key already exists
	bool insert(const Entry &e);
	// insert element into hashmap by key and its data
	bool insert(const TKey key, const TData data);

	// remove a key from hashtable
	// true if removed, false if not found
	bool remove(TKey key);

	// search for the data associated with the given key
	// returns pointer to key's associated data if found, if not found, returns nullptr
	TData *find(TKey key) const;

	// number of entries hashtable is storing
	unsigned entries() const;


protected:
	// hashtable body
	std::list<Entry> *Table;
	unsigned TableSize;

	// hashing function, must convert key to an unsigned int
	unsigned(*HasherFunc)(const TKey &);

	// number of elements hashmap is storing
	unsigned EntriesCount;

	// longest chain, used to resize hashmap
	unsigned MaxChained;

protected:
	// resizes hashmap if chains are growing too long
	void resize_();
};



template<typename TKey, typename TData>
ChainedHashTable<TKey, TData>::ChainedHashTable(unsigned(*hasher)(const TKey &), unsigned size)
	: HasherFunc(hasher), Table(new std::list<Entry>[size]), TableSize(size), EntriesCount(0), MaxChained(0)
{}

template<typename TKey, typename TData>
inline ChainedHashTable<TKey, TData>::ChainedHashTable(const ChainedHashTable &map)
	: HasherFunc(map.HasherFunc), Table(new std::list<Entry>[map.TableSize]), TableSize(map.TableSize),
	EntriesCount(map.EntriesCount), MaxChained(map.MaxChained)
{
	for (unsigned i = 0; i < TableSize; i++) {
		std::list<Entry> &list = map.Table[i];
		for (auto it = list.begin(); it != list.end(); ++it)
			Table[HasherFunc((*it).key) % TableSize].push_back((*it));
	}
}


template<typename TKey, typename TData>
ChainedHashTable<TKey, TData>::~ChainedHashTable() {
	delete[] Table;
}



template<typename TKey, typename TData>
inline bool ChainedHashTable<TKey, TData>::insert(const Entry &e) {
	std::list<Entry> &chosenList = Table[HasherFunc(e.key) % TableSize];
	// ensure key doesnt exist in table
	for (auto it = chosenList.begin(); it != chosenList.end(); ++it) {
		if ((*it).Key == e.Key)
			return false;
	}
	// insert entry into table
	chosenList.push_back(e);

	// clean up table if needed
	MaxChained = (chosenList.size() > MaxChained) ? chosenList.size() : MaxChained;
	EntriesCount++;

	resize_();
	return true;
}


template<typename TKey, typename TData>
inline bool ChainedHashTable<TKey, TData>::insert(const TKey key, const TData data) {
	return insert(Entry(key, data));
}


template<typename TKey, typename TData>
inline bool ChainedHashTable<TKey, TData>::remove(TKey key) {
	std::list<Entry> &chosenList = Table[HasherFunc(key) % TableSize];

	for (auto it = chosenList.begin(); it != chosenList.end(); ++it) {
		if ((*it).Key == key) {
			chosenList.erase(it);
			--EntriesCount;
			return true;
		}
	}
	return false;
}


template<typename TKey, typename TData>
inline TData * ChainedHashTable<TKey, TData>::find(TKey key) const {
	std::list<Entry> &chosenList = Table[HasherFunc(key) % TableSize];

	for (auto it = chosenList.begin(); it != chosenList.end(); ++it) {
		if ((*it).Key == key)
			return &(*it).data;
	}
	return nullptr;
}


template<typename TKey, typename TData>
inline unsigned ChainedHashTable<TKey, TData>::entries() const {
	return EntriesCount;
}


template<typename TKey, typename TData>
inline void ChainedHashTable<TKey, TData>::resize_() {
	if (MaxChained > MAX_CHAINED_DATA) {
		unsigned newTableSize = (unsigned)(EntriesCount * BASE_INCREASE);
		std::list<Entry> *newTable = new std::list<Entry>[newTableSize];

		for (unsigned i = 0; i < TableSize; i++) {
			std::list<Entry> &originList = Table[i];
			for (auto it = originList.begin(); it != originList.end(); ++it)
				newTable[HasherFunc((*it).Key) % newTableSize].push_back((*it));
		}

		MaxChained = 1;
		delete[] Table;
		Table = newTable;
		TableSize = newTableSize;
	}
}



#endif
