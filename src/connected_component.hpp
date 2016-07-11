#ifndef CONNECTED_COMPONENT_HPP
#define CONNECTED_COMPONENT_HPP

#include "pw_alignment.hpp"

#include "alignment_index.hpp"

//#include "IntervalTree.hpp"

//#include <boost/graph/biconnected_components.hpp>
#include <map>
#include <vector>
#include <cassert>
#include <omp.h>
#include <set>

//#define FRACTION 0.5


class compute_cc_avl_fraction {
	public:
	compute_cc_avl_fraction(const std::set<const pw_alignment*, compare_pointer_pw_alignment> & als_in, size_t num_sequences, size_t num_threads): alind(NULL) {//Is used to create partially overlapped connected components
		this->num_threads = num_threads;
		std::cout << " CC AVL build index on " << als_in.size() << std::endl << std::flush;
		std::multimap<size_t, const pw_alignment *> sorter;
		for(std::set<const pw_alignment*, compare_pointer_pw_alignment>::const_iterator it = als_in.begin(); it!=als_in.end(); it++){
			const pw_alignment * al = *it;
			als.push_back(al);
			remained_als.insert(al);
			size_t len = al->alignment_length();
			sorter.insert(std::make_pair(len, al));
		}
		std::cout << " Sorting done " << sorter.size()<< std::endl;
		std::cout << "remainder size" << remained_als.size() << std::endl;
		std::vector<const pw_alignment *> sorted_als(sorter.size());
		size_t num = 0;
		for(std::multimap<size_t, const pw_alignment *>::iterator it = sorter.begin(); it!=sorter.end(); ++it) {
			const pw_alignment * al = it->second;
			sorted_als.at(num) = al;
			alignments.insert(al);
		//	remained_als.insert(al);
		//	als.push_back(al);
		//	als_index.insert(std::make_pair(al,num));
			num++;
		}
		alind = new alignment_index(num_sequences, num_threads, sorted_als);
	//	alind->debug_print();
	}
	compute_cc_avl_fraction(std::set< pw_alignment, compare_pw_alignment> & als_in, size_t num_sequences, size_t num_threads): alind(NULL) {//Is used to create partially overlapped connected components
		this->num_threads = num_threads;
		std::cout << " CC AVL build index on " << als_in.size() << std::endl << std::flush;
		std::multimap<size_t, const pw_alignment *> sorter;
		for(std::set<pw_alignment, compare_pw_alignment>::iterator it = als_in.begin(); it!=als_in.end(); it++){
			const pw_alignment * al = &*it;
			als.push_back(al);
			remained_als.insert(al);
		//	remainders_als.insert(*it);
			size_t len = al->alignment_length();
			sorter.insert(std::make_pair(len, al));
		}
		std::cout << " Sorting done " << sorter.size()<< std::endl;
		std::cout << "remainder size" << remained_als.size() << std::endl;
		std::vector<const pw_alignment *> sorted_als(sorter.size());
		size_t num = 0;
		for(std::multimap<size_t, const pw_alignment *>::iterator it = sorter.begin(); it!=sorter.end(); ++it) {
			const pw_alignment * al = it->second;
			sorted_als.at(num) = al;
			alignments.insert(al);
		//	remained_als.insert(al);
		//	als.push_back(al);
		//	als_index.insert(std::make_pair(al,num));
			num++;
		}
		alind = new alignment_index(num_sequences, num_threads, sorted_als);
	//	alind->debug_print();
	}

	~compute_cc_avl_fraction(){
		delete alind;
	}
	void non_recursive_compute(std::set< const pw_alignment* , compare_pointer_pw_alignment> &, std::set<const pw_alignment* , compare_pointer_pw_alignment> & , double & );
	void cc_step_non_recursive(const pw_alignment*  ,  size_t & , size_t & , size_t &, std::set< const pw_alignment* , compare_pointer_pw_alignment>& , double &  , std::set<const pw_alignment* , compare_pointer_pw_alignment>&);
	void compute(std::vector<std::set< const pw_alignment* , compare_pointer_pw_alignment> > &);
	void get_cc(const pw_alignment & , std::set <const pw_alignment*, compare_pointer_pw_alignment> & , std::set <const pw_alignment*, compare_pointer_pw_alignment> & );
	void cc_step(const pw_alignment &, size_t , size_t , size_t , std::set <const pw_alignment*, compare_pointer_pw_alignment> & , std::set <const pw_alignment* , compare_pointer_pw_alignment>  & );
	void compute_fraction(std::vector<const pw_alignment*> &, const double  ,  const size_t & , const size_t & , const size_t &,std::vector<const pw_alignment*> & );
	void node_adjucents(const pw_alignment*,std::set<const pw_alignment*>&)const;
	void node_adjucents(const pw_alignment*,std::vector<const pw_alignment*>&)const;
	size_t get_id (const pw_alignment*)const;
	void test_redundancy(const pw_alignment* , std::set< const pw_alignment* , compare_pointer_pw_alignment> & );
	std::set<const pw_alignment*, compare_pointer_pw_alignment> get_remaining_als()const;
//	const std::set<pw_alignment, compare_pw_alignment> get_remainders()const;
	private:
	
	std::set<const pw_alignment*, compare_pointer_pw_alignment> alignments; 
	std::vector<const pw_alignment*> als;
	std::set<const pw_alignment*, compare_pointer_pw_alignment> remained_als; 
//	std::set<pw_alignment, compare_pw_alignment> remainders_als; 

	alignment_index  * alind;
	size_t num_threads;
//	std::map<const pw_alignment* , size_t> als_index;
	std::multimap<const pw_alignment* , const pw_alignment*>edges; 
//	std::set<std::pair<const pw_alignment*, const pw_alignment*> > edges;

	size_t edge_size;
	std::set<const pw_alignment*, compare_pointer_pw_alignment> seen1; 



};
//This class was initially written to find articulation points and make biconnected graph components. IT is able to find the articulations point correctly but fills in the 'stack' with wrong nodes. In case of being used should be fixed.
class biconnected_component{
	public:
	biconnected_component(compute_cc_avl_fraction & cc, std::vector<std::set< const pw_alignment* , compare_pointer_pw_alignment> >& als):cc_fraction(cc), visited(als.size()), low(als.size()), Depth(als.size()), parent(als.size()),alignments(als.size()),als_index(als.size()){
		depth = 0;
		std::cout << "als size "<< als.size()<<std::endl;
		for(size_t i =0; i < als.size();i++){
			size_t index=0;
			for(std::set<const pw_alignment*, compare_pointer_pw_alignment>::iterator it = als.at(i).begin(); it !=als.at(i).end();it++){
				alignments.at(i).push_back(*it);
				als_index.at(i).insert(std::make_pair(*it,index));
				index++;
			}
			visited.at(i).resize(index);
			Depth.at(i).resize(index);
			low.at(i).resize(index);
			parent.at(i).resize(index);
		}		
	}
	~biconnected_component(){}
	void creat_biconnected_component(size_t & );
	void get_articulation_points(size_t &, size_t node);


	private:
	compute_cc_avl_fraction & cc_fraction;
	size_t depth;
	std::vector<std::set<const pw_alignment*, compare_pointer_pw_alignment> > stack;
	std::vector<std::vector<const pw_alignment*> >alignments;//The node indices are their row in the alignments
	std::vector<std::map<const pw_alignment* , size_t> >als_index;
	std::vector<std::vector<bool> >visited;
	std::set<const pw_alignment*, compare_pointer_pw_alignment> temp;
	std::vector<std::vector<size_t> >low;
	std::vector<std::vector<size_t> >Depth;
	std::vector<std::vector<size_t> >parent;
};
//2-edge-connected-component:
//Note that each set of als is a single component graph
class two_edge_cc{
	public:
	two_edge_cc(compute_cc_avl_fraction & cc, std::set< const pw_alignment* , compare_pointer_pw_alignment> & als):cc_fraction(cc){
		count = 0;
		bridge = false;
	//	std::cout << "als are"<<std::endl;
		for(std::set<const pw_alignment*, compare_pointer_pw_alignment>::iterator it = als.begin(); it !=als.end();it++){
			alignments.push_back(*it);
		//	std::cout<< *it<<std::endl;
			visited_als.insert(std::make_pair(*it,-1));
			als_low.insert(std::make_pair(*it,-1));
		}
		std::cout<< "visit size "<< visited_als.size() << "low size "<< als_low.size() << " al size "<< alignments.size() <<std::endl;
	}
	two_edge_cc(compute_cc_avl_fraction & cc):cc_fraction(cc){


	}
	void find_bridges(std::vector<std::set<const pw_alignment*, compare_pointer_pw_alignment> >& , std::set<pw_alignment, compare_pw_alignment> &);
	void deep_first_search(const pw_alignment*, const pw_alignment*,std::set<pw_alignment, compare_pw_alignment> &);
	~two_edge_cc(){}
	private:
	compute_cc_avl_fraction & cc_fraction;	
	std::vector<const pw_alignment*> alignments;
	std::map<const pw_alignment*, int> visited_als;
	std::map<const pw_alignment*, int> als_low;
	size_t count;
	bool bridge;

	std::set<const pw_alignment*,compare_pointer_pw_alignment> temp;
	std::vector<std::set<const pw_alignment*,compare_pointer_pw_alignment> > stack;
	std::set<const pw_alignment*,compare_pointer_pw_alignment> remainder;


	








};








#endif