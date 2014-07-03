#ifndef PW_ALIGNMENT_HPP
#define PW_ALIGNMENT_HPP

#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <cassert>
#include <cstdlib>
using namespace std;

class pw_alignment {
	public:
	
	pw_alignment(string sample1, string sample2, size_t sample1_begin, size_t sample2_begin, size_t sample1_end, size_t sample2_end,size_t sample1reference, size_t sample2reference);
	pw_alignment();
	~pw_alignment();
	pw_alignment(const pw_alignment & p);


	size_t alignment_length() const;
	void alignment_col(size_t c, char & s1, char & s2) const;
	vector<bool> getsample1()const;
	vector<bool> getsample2()const;
	size_t getbegin1()const;
	size_t getbegin2() const;
	size_t getend1()const;
	size_t getend2()const;
	size_t getreference1() const;
	size_t getreference2() const;
	void split(bool sample, size_t position, pw_alignment & first_part, pw_alignment & second_part) const;
	void set_alignment_bits(vector<bool> s1, vector<bool> s2);
	vector<bool> getsample(size_t id)const;
	size_t getbegin(size_t id)const;
	size_t getend(size_t id)const;
	size_t getreference(size_t id)const;
	
	void setbegin1(size_t begin1);
	void setbegin2(size_t begin2);
	
	void setend1(size_t end1);
	void setend2(size_t end2);

	void print() const;

	private:
	vector<vector<bool> > samples;
	vector<size_t> begins;
	vector<size_t> ends;
	vector<size_t> references;
	
	static inline void base_translate(char base, bool & bit1, bool & bit2, bool & bit3);
	static inline char base_translate_back(bool bit1, bool bit2, bool bit3);
};


class compare_pw_alignment {
	public:
	bool operator()(const pw_alignment *const &a, const pw_alignment *const &b);
};
		
#endif 



