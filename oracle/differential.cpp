#include "include/leaderboard.hpp"
#include "oracle/reference.hpp"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace std;
using namespace leaderboard;
struct Op{
    int64_t id,score;
    bool remove;
};
static void require(bool value,const string&what){
    if(!value){
        throw logic_error(what);
    }
}
template<class Exception,class Action>
static void expect_throw(Action action,const string&what){
    try{
        action();
    }catch(const Exception&){
        return;
    }
    throw logic_error(what);
}
static void compare_all(const Leaderboard&actual,const Reference&expected){
    require(actual.size()==expected.size(),"full size");
    require(actual.audit()==expected.size(),"full audit");
    auto x=actual.positions(1,expected.size()+2);
    auto y=expected.positions(1,expected.size()+2);
    require(x.size()==y.size(),"full range size");
    for(size_t i=0;i<y.size();i++){
        require(same(x[i],y[i]),"full range order");
        require(same(actual.select(i+1),y[i]),"full select");
        require(actual.rank(y[i].player_id)==expected.rank(y[i].player_id),"full rank");
    }
}
static void targeted(){
    Leaderboard actual({-10,0,10,20});
    Reference expected;
    compare_all(actual,expected);
    expect_throw<out_of_range>([&]{actual.select(1);},"empty select");
    expect_throw<out_of_range>([&]{expected.select(1);},"oracle empty select");
    expect_throw<out_of_range>([&]{actual.rank(99);},"missing rank");
    expect_throw<out_of_range>([&]{expected.rank(99);},"oracle missing rank");
    expect_throw<out_of_range>([&]{actual.remove(99);},"missing remove");
    expect_throw<out_of_range>([&]{expected.remove(99);},"oracle missing remove");
    expect_throw<invalid_argument>([&]{actual.revise(-1,0);},"negative ID");
    expect_throw<invalid_argument>([&]{expected.revise(-1,0);},"oracle negative ID");
    for(auto e:vector<Entry>{{20,7},{10,9},{10,2},{-10,4},{0,0}}){
        actual.revise(e.player_id,e.score);
        expected.revise(e.player_id,e.score);
    }
    compare_all(actual,expected);
    require(expected.rank(2)==2 && expected.rank(9)==2,"competition ties");
    require(expected.select(2).player_id==2,"ID tie order");
    for(int threshold:{-20,10,30}){
        auto parts=actual.split(threshold);
        Reference high,low;
        for(auto e:expected.positions(1,expected.size())){
            (e.score>threshold?high:low).revise(e.player_id,e.score);
        }
        require(actual.size()==0 && actual.audit()==0,"split consumes source");
        compare_all(parts.first,high);
        compare_all(parts.second,low);
        parts.first.merge(parts.second);
        require(parts.second.size()==0 && parts.second.audit()==0,"merge consumes source");
        compare_all(parts.first,expected);
        actual=move(parts.first);
    }
    actual.revise(9,20);
    expected.revise(9,20);
    actual.revise(9,20);
    expected.revise(9,20);
    compare_all(actual,expected);
    expect_throw<out_of_range>([&]{actual.select(0);},"zero position");
    expect_throw<out_of_range>([&]{expected.select(0);},"oracle zero position");
    expect_throw<out_of_range>([&]{actual.select(6);},"past-end position");
    expect_throw<out_of_range>([&]{expected.select(6);},"oracle past-end position");
    expect_throw<invalid_argument>([&]{actual.positions(2,1);},"reversed range");
    expect_throw<invalid_argument>([&]{expected.positions(2,1);},"oracle reversed range");
    require(actual.positions(6,9).empty() && expected.positions(6,9).empty(),"past-end range");
    for(auto e:expected.positions(1,expected.size())){
        actual.remove(e.player_id);
        expected.remove(e.player_id);
        compare_all(actual,expected);
    }
}
static bool replay(const vector<Op>&ops,bool audit_each,string*failure){
    try{
        Leaderboard actual({0,1,2,3,4,5,6,7,8,9,10});
        Reference expected;
        for(size_t i=0;i<ops.size();i++){
            const auto&op=ops[i];
            if(op.remove){
                // Missing IDs are legal in generated and reduced traces. Compare
                // each result independently so one exception cannot hide a mismatch.
                bool actual_missing=false,expected_missing=false;
                try{
                    actual.remove(op.id);
                }catch(const out_of_range&){
                    actual_missing=true;
                }
                try{
                    expected.remove(op.id);
                }catch(const out_of_range&){
                    expected_missing=true;
                }
                require(actual_missing==expected_missing,"remove result at step "+to_string(i));
            }else{
                actual.revise(op.id,op.score);
                expected.revise(op.id,op.score);
            }
            require(actual.size()==expected.size(),"size at step "+to_string(i));
            if(actual.size()>0){
                auto pos=1+i%actual.size();
                auto a=actual.select(pos),b=expected.select(pos);
                require(same(a,b),"select at step "+to_string(i));
                require(actual.rank(a.player_id)==expected.rank(a.player_id),"rank at step "+to_string(i));
                if(i%31==0){
                    auto x=actual.positions(1,min<size_t>(10,actual.size()));
                    auto y=expected.positions(1,min<size_t>(10,expected.size()));
                    require(x.size()==y.size(),"range size");
                    for(size_t j=0;j<x.size();j++){
                        require(same(x[j],y[j]),"range item");
                    }
                }
            }
            if(audit_each || i%1000==0){
                actual.audit();
            }
        }
        compare_all(actual,expected);
        return true;
    }catch(const exception&e){
        if(failure!=nullptr){
            *failure=e.what();
        }
        return false;
    }
}
static vector<Op> shrink(vector<Op>ops){
    size_t chunks=2;
    while(ops.size()>=2){
        bool reduced=false;
        auto width=(ops.size()+chunks-1)/chunks;
        for(size_t start=0;start<ops.size();start+=width){
            auto candidate=ops;
            candidate.erase(candidate.begin()+start,candidate.begin()+min(start+width,ops.size()));
            if(!candidate.empty() && !replay(candidate,true,nullptr)){
                ops=move(candidate);
                chunks=max<size_t>(2,chunks-1);
                reduced=true;
                break;
            }
        }
        if(!reduced){
            if(chunks>=ops.size()){
                break;
            }
            chunks=min(ops.size(),chunks*2);
        }
    }
    return ops;
}
int main(int argc,char**argv){
    try{
        size_t operations=10000;
        size_t ids=1000;
        uint64_t seed=253;
        if(argc>1){
            operations=stoull(argv[1]);
        }
        if(argc>2){
            ids=stoull(argv[2]);
        }
        if(argc>3){
            seed=stoull(argv[3]);
        }
        if(ids==0){
            throw invalid_argument("IDs must be positive");
        }
        targeted();
        mt19937_64 rng(seed);
        vector<Op>ops;
        ops.reserve(operations);
        for(size_t i=0;i<operations;i++){
            ops.push_back({static_cast<int64_t>(rng()%ids),static_cast<int64_t>(rng()%11),rng()%7==0});
        }
        string failure;
        if(!replay(ops,false,&failure)){
            auto small=shrink(move(ops));
            cerr << "FAIL " << failure << "; reduced failing sequence:\n";
            for(size_t i=0;i<small.size();i++){
                const auto&op=small[i];
                if(op.remove){
                    cerr << "remove " << op.id << '\n';
                }else{
                    cerr << "revise " << op.id << " " << to_string(op.score) << '\n';
                }
            }
            return 1;
        }
        cout << "PASS targeted cases; " << operations << " operations, seed " << seed << '\n';
        return 0;
    }catch(const exception&e){
        cerr << "test error: " << e.what() << '\n';
        return 1;
    }
}
