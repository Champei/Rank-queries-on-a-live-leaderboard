#include "oracle/reference.hpp"
#include <algorithm>
#include <stdexcept>
using namespace std;
namespace leaderboard{
    void Reference::remove(int64_t id){
        auto it=find_if(data_.begin(),data_.end(),[id](Entry e){
            return e.player_id==id;
        });
        if(it==data_.end()){
            throw out_of_range("unknown player");
        }
        data_.erase(it);
    }
    void Reference::revise(int64_t id,int64_t score){
        if(id<0){
            throw invalid_argument("negative player ID");
        }
        auto it=find_if(data_.begin(),data_.end(),[id](Entry e){
            return e.player_id==id;
        });
        if(it!=data_.end()){
            data_.erase(it);
        }
        Entry e{score,id};
        data_.insert(lower_bound(data_.begin(),data_.end(),e,[](Entry a,Entry b){
            return a.score>b.score || (a.score==b.score && a.player_id<b.player_id);
        }),e);
    }
    size_t Reference::rank(int64_t id) const{
        auto it=find_if(data_.begin(),data_.end(),[id](Entry e){
            return e.player_id==id;
        });
        if(it==data_.end()){
            throw out_of_range("unknown player");
        }
        size_t higher=0;
        while(higher<data_.size() && data_[higher].score>it->score){
            higher++;
        }
        return higher+1;
    }
    Entry Reference::select(size_t pos) const{
        if(pos<1 || pos>data_.size()){
            throw out_of_range("position");
        }
        return data_[pos-1];
    }
    vector<Entry> Reference::positions(size_t first,size_t last) const{
        if(first<1 || last<first){
            throw invalid_argument("rank range");
        }
        if(first>data_.size()){
            return {};
        }
        last=min(last,data_.size());
        return {data_.begin()+(first-1),data_.begin()+last};
    }
}
