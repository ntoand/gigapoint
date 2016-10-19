#ifndef _LRU_H_
#define _LRU_H_

#include "NodeGeometry.h"

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>
#include <iostream>
#include <exception>

using namespace std;

// a double linked list node
struct Node {
	Node* prev;
	Node* next;
	string key;
	NodeGeometry* value;

	Node(const string& keyObj, NodeGeometry* valueObj): prev(0), next(0), key(keyObj) {
		value = valueObj;
	}

	virtual ~Node() {
		cleanup();
	}

	void cleanup() {
		if (next) {
			delete next;
		}
		next = 0;
		prev = 0;
	}

	void unlink() {
		if (next) {
			next->prev = prev;
		}
		if (prev) {
			prev->next = next;
		}
		next = 0;
		prev = 0;
	}

	template<class Visitor>
	void walk(Visitor& visitorFunc) {
		visitorFunc(*this);
		if (this->next) {
			this->next->walk(visitorFunc);
		}
	}
};

// a doubly linked list class
struct List {
	Node* head;
	Node* tail;
	size_t size;

	List() :
			head(0), tail(0), size(0) {
	}

	virtual ~List() {
		clear();
	}

	void clear() {
		if (head) {
			delete head;
		}
		head = 0;
		tail = 0;
		size = 0;
	}

	Node* pop() {
		if (!head) {
			return 0;
		} else {
			Node* newHead = head->next;
			head->unlink();
			Node* oldHead = head;
			head = newHead;
			size--;
			if (size == 0) {
				tail = 0;
			}
			return oldHead;
		}
	}

	Node* remove(Node* node) {
		if (node == head) {
			head = node->next;
		}
		if (node == tail) {
			tail = node->prev;
		}
		node->unlink();
		size--;
		return node;
	}

	void push(Node* node) {
		node->unlink();
		if (!head) {
			head = node;
		} else if (head == tail) {
			head->next = node;
			node->prev = head;
		} else {
			tail->next = node;
			node->prev = tail;
		}
		tail = node;
		size++;
	}
};


#define MapType map<string, Node*>

class LRUCache {
public:
	class KeyNotFound: public std::exception {
	public:
		const char* what() const throw () {
			return "KeyNotFound";
		}
	};

	// -- methods
	LRUCache(size_t maxSize = 64, size_t elasticity = 10) :
			m_maxSize(maxSize), m_elasticity(elasticity) {
	}

	virtual ~LRUCache() {
	}

	void clear() {
		m_cache.clear();
		m_keys.clear();
	}

	void insert(const string& key, NodeGeometry* value) {
		MapType::iterator iter = m_cache.find(key);
		if (iter != m_cache.end()) {
			iter->second->value = value;
			m_keys.remove(iter->second);
			m_keys.push(iter->second);
		} else {
			Node* n = new Node(key, value);
			m_cache[key] = n;
			m_keys.push(n);
			prune();
		}

	}

	bool tryGet(const string& key, NodeGeometry*& value) {
		MapType::iterator iter = m_cache.find(key);
		if (iter == m_cache.end()) {
			return false;
		} else {
			m_keys.remove(iter->second);
			m_keys.push(iter->second);
			value = iter->second->value;
			return true;
		}

	}

	const NodeGeometry* get(const string& key) {
		MapType::iterator iter = m_cache.find(key);
		if (iter == m_cache.end()) {
			throw KeyNotFound();
		}
		m_keys.remove(iter->second);
		m_keys.push(iter->second);
		return iter->second->value;

	}

	void remove(const string& key) {
		MapType::iterator iter = m_cache.find(key);
		if (iter != m_cache.end()) {
			m_keys.remove(iter->second);
			m_cache.erase(iter);
		}
	}

	bool contains(const string& key) {
		return m_cache.find(key) != m_cache.end();
	}

	static void printVisitor(const Node& node) {
		std::cout << "{" << node.key << ":" << node.value << "}" << std::endl;
	}

	void dumpDebug() const {
		std::cout << "LRUCache Size : " << m_cache.size() << " (max:" << m_maxSize
				<< ") (elasticity: " << m_elasticity << ")" << std::endl;
	}

	void dumpDebug(std::ostream& os) const {
		std::cout << "LRUCache Size : " << m_cache.size() << " (max:" << m_maxSize
				<< ") (elasticity: " << m_elasticity << ")" << std::endl;
		for (Node* node = m_keys.head; node != NULL; node = node->next) {
			printVisitor(*node);
		}
	}

protected:
	size_t prune() {
		if (m_maxSize > 0 && m_cache.size() >= (m_maxSize + m_elasticity)) {
			size_t count = 0;
			while (m_cache.size() > m_maxSize) {
				Node* n = m_keys.pop();
				n->value->freeData();
				m_cache.erase(n->key);
				delete n;
				count++;
				std::cout << "LRU free data " << std::endl;
			}
			return count;
		} else {
			return 0;
		}
	}

private:
	MapType m_cache;
	List m_keys;
	size_t m_maxSize;
	size_t m_elasticity;

private:
	LRUCache(const LRUCache&);
	const LRUCache& operator =(const LRUCache&);
};

#endif
