#ifndef TABLE_HPP
#define TABLE_HPP

#include <unordered_map>
#include <array>

class RepetitionTable {
private:
	std::unordered_map<uint64_t, uint8_t> table;

public:
	void store(uint64_t boardHash) {
		auto it = this->table.find(boardHash);

		if (it != this->table.end()) {
			it++;
		}
		else {
			this->table[boardHash] = 1;
		}
	}
	bool isThreefoldRepetition(uint64_t boardHash) const {
		auto it = this->table.find(boardHash);
		if (it != this->table.end()) {
			return it->second >= 3;
		}
		return false;
	}
	void remove(uint64_t boardHash) {
		auto it = this->table.find(boardHash);
		if (it != this->table.end()) {
			if (it->second > 1) {
				it->second--;
			}
			else {
				this->table.erase(it);
			}
		}
	}
	void eraseTable() {
		this->table.clear();
	}
};

class RepetitionTable2 {
private:
	std::array<uint64_t, 50> table;
	int size;

public:
	RepetitionTable2() {
		this->table.fill(0);
		this->size = 0;
	}

	void store(uint64_t boardHash) {
		this->table[this->size++] = boardHash;
	}
	void remove(uint64_t boardHash) {
		for (int i = 0; i < size; ++i) {
			if (table[i] == boardHash) {
				table[i] = table[size - 1];
				--size;
				break;
			}
		}
	}
	void eraseTable() {
		this->size = 0;
	}
	bool isThreefoldRepetition(uint64_t boardHash) const {
		uint8_t count = 0;

		for (int i = 0; i < this->size; i++) {
			if (this->table[i] == boardHash) {
				count++;
				if (count >= 3) {
					return true;
				}
			}
		}
		return false;	
	}
};



#endif