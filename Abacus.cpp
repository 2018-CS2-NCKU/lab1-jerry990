#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <set>
#include <sstream>
#include <time.h>
#include <algorithm>
using namespace std;
set <int>::iterator it;
class shape{
	public:
		int x;
		int y;
		int h;
		int w; 
};
class node{
	public:
		int index;
		string name;
		int row_num;
		int w;
		int h;
		int x;
		int y;
		double nx;
		double ny;
		int ifter;
		vector <shape> s;
	};
class terninode{
	public:
		int index;
		string name;
		int w;
		int h;
		int x;
		int y;
};
class net_in_cell{
	public:
		vector <int> net;
};
class net{
	public:
		int index;
		string name;
		set <int> node;	
		set <int> ninode;	 
};
class range{
	public:
		double lx;
		double rx;
};
class cluster{
	public:
		int row_num;
		double x;
		int w;
		vector <node> n;
};
class row{
	public:
		int index;
		int x;//coordinate in left-bottom 
		int y;//coordinate in left-bottom 
		int h;
		int sidewidth;
		int sitespace;
		int numsite;
		vector <range> r;
		vector <cluster> c;
};
class tile{
	public:
		int x;
		int w;
		int row_num;
};
vector <tile> intemp; 
vector <node> x,stand,post_block;//post_block 存放之後要處理的block
vector <terninode> y;
vector <net> netarray; 
vector <row>  r;//save存放為改變的row 
vector <net_in_cell> o_cellarray,p_cellarray; 
int bound=0;
int leftx=0,lowy=0,rightx=0,uppery=0,placeheight=0;//框架的範圍 
bool xn_sort(node lh,node rh){
	return lh.x<rh.x;
}
void readnode(string s){
	fstream fin;
	string s1;
	fin.open(s.c_str(),ios::in);
	int count=0;
	while(getline(fin,s1)){
		count++;
		if(count==4)
			break;
	}
	int cell=0,terminal=0;
	fin>>s1>>s1>>cell;
	fin>>s1>>s1>>terminal;
	bound=cell-terminal;
	node temp;
	for(int i=0;i<cell-terminal;i++){
		temp.index=i;
		temp.ifter=0;
		fin>>temp.name>>temp.w>>temp.h;
		x.push_back(temp);
   	}
	terninode temp1;
	int local=0;//terminal和termminalni的界線
	string s2;//tername
	for(int i=0;i<terminal;i++){
		temp.index=cell-terminal+i;
		fin>>temp.name>>temp.w>>temp.h>>s2;
		temp.ifter=1;
		x.push_back(temp);
		if(strcmp(s2.c_str(),"terminal_NI")==0){
			local=i;
			break;
		}
    }
	temp1.name=x[x.size()-1].name;
	temp1.w=x[x.size()-1].w;
	temp1.h=x[x.size()-1].h;
	temp1.index=0;
	y.push_back(temp1);
	x.pop_back();//x完成
	int index=1;
	for(int i=local+1;i<terminal;i++){
		fin>>temp1.name>>temp1.w>>temp1.h>>s2;	
		y.push_back(temp1);
		index++;
	} 
	fin.close();
}
void readlegal(string s){
	fstream fin;
	string s1;//store the string not use
	fin.open(s.c_str(),ios::in);
	int count=0;
	while(getline(fin,s1)){
		count++;
		if(count==4)
			break;
	}
	for(int i=0;i<x.size();i++){
		fin>>s1>>x[i].x>>x[i].y>>s1>>s1;
		x[i].nx=x[i].x;
	}
	for(int i=0;i<y.size();i++){
		fin>>s1>>y[i].x>>y[i].y>>s1>>s1;
	}
}
void readscl(string s){
	fstream fin;
	string s1;//store the string not use
	fin.open(s.c_str(),ios::in);
	int count=0,numrows=0;
	while(getline(fin,s1)){
		count++;
		if(count==4)
			break;
	}
	fin>>s1>>s1>>numrows;
	row temp;
	for(int i=0;i<numrows;i++){
		temp.index=r.size();
		fin>>s1>>s1>>s1>>s1>>temp.y;//y coordinate
		fin>>s1>>s1>>temp.h;
		fin>>s1>>s1>>temp.sidewidth;
		fin>>s1>>s1>>temp.sitespace;
		fin>>s1>>s1>>s1;
		fin>>s1>>s1>>s1;
		fin>>s1>>s1>>temp.x>>s1>>s1>>temp.numsite;
		fin>>s1;
		r.push_back(temp);
	}
	fin.close();
}
void readshape(string s){
	fstream fin;
	string s1;//store the string not use
	int nshape,num;
	int count=0;
	fin.open(s.c_str(),ios::in);
	while(getline(fin,s1)){
		count++;
		if(count==4)
			break;
	}
	fin>>s1>>s1>>nshape;
	string name;
	int n;
	stringstream ss;
	shape temp;
	while(fin>>name>>s1>>num){
		if(name[0]=='o')
			name.erase(0,1);
		else
			name.erase(0,name.find_last_of("/")+2);
		ss<<name;
		ss>>n;
		for(int i=0;i<num;i++){
			fin>>s1>>temp.x>>temp.y>>temp.w>>temp.h;
			(x[n].s).push_back(temp);
		}
		ss.clear();
	}
	fin.close();
}
void encodetile(){
	for(int i=bound;i<x.size();i++){
		if(x[i].s.size()==0){
			int start=(x[i].y-lowy)/placeheight;
			int end=x[i].h/placeheight+start;
			tile temp;
			for(int j=start;j<end;j++){
				temp.x=x[i].x;
				temp.w=x[i].w;
				temp.row_num=j;
				intemp.push_back(temp);
			}
		}//長方形的terminal 
		else{
			tile temp;
			for(int j=0;j<x[i].s.size();j++){
				temp.row_num=(x[i].y-lowy)/placeheight;
				temp.w=x[i].s[j].w;
				temp.x=x[i].s[j].x;
				intemp.push_back(temp);
			}
		}//特殊形狀的terminal 
	}
}
void readnet(string s){
	fstream fin;
	string s1;//store the string not use
	int num,numnets,numpins;//cell數
	string nets;
	int index=0;
	fin.open(s.c_str(),ios::in);
	int count=0;
	while(getline(fin,s1)){
		count++;
		if(count==4)
			break;
	}
	fin>>s1>>s1>>numnets;
	fin>>s1>>s1>>numpins;
	net temp;
	string s2;
	int n;
	int n1;
	int a=0;
	int n2;
	stringstream ss,ss1;
    while(fin>>s1>>s1>>num>>nets){
		temp.index=index;
		temp.name=nets;
		nets.erase(0,1);
		ss1<<nets;
		ss1>>n2;
		for(int i=0;i<num;i++){
			fin>>s2;
			if(s2[0]=='o'){
				a=1;
				s2.erase(0,1);
			}
			else if(s2[0]=='p'){
				a=2;
				s2.erase(0,1);
			}
			else{
				n1=s2.find_last_of("/");
				if(s2[n1+1]=='o')
					a=1;
				else if(s2[n1+1]=='p')
					a=2;		
				s2.erase(0,n1+2);
			}
			ss<<s2;
			ss>>n;
			ss.clear();
			if(a==1){
				(o_cellarray[n].net).push_back(n2);
				(temp.node).insert(n);
			}
			else if(a==2){
				(p_cellarray[n].net).push_back(n2);
				(temp.ninode).insert(n);
			}
			fin>>s1>>s1>>s1>>s1;
    		a=0;
		 }
		ss1.clear();
        netarray.push_back(temp);
		(temp.node).clear();
		(temp.ninode).clear();
		index++;
	}
	fin.close();
}
bool x_sort(tile lh,tile rh){
	return lh.x<rh.x;
}
void cut_range(){
	leftx=r[0].x;
	lowy=r[0].y;
	rightx=r[0].x+r[0].numsite;
	uppery=r[0].y+r[0].h;
	placeheight=r[0].h;
	encodetile();//對每個solid tile做編碼 
//	sort(intemp.begin(),intemp.end(),tile_sort);
	vector <tile> intemp1[r.size()];
	for(int i=0;i<intemp.size();i++){
		intemp1[intemp[i].row_num].push_back(intemp[i]);
	}
	for(int i=0;i<r.size();i++){
		sort(intemp1[i].begin(),intemp1[i].end(),x_sort);
	}
	range temp1;
	for(int i=0;i<r.size();i++){
		if(intemp1[i].size()==0){
			temp1.lx=leftx;
			temp1.rx=rightx;
			r[i].r.push_back(temp1);
			continue;
		}
		if(intemp1[i][0].x!=leftx){
			temp1.lx=leftx;
			temp1.rx=intemp1[i][0].x;
			r[i].r.push_back(temp1);
		}
		for(int j=0;j<intemp1[i].size()-1;j++){
			if(intemp1[i][j].x+intemp1[i][j].w==intemp1[i][j+1].x)
				continue;//兩個TILE之間是重合的 
			else{
				temp1.lx=intemp1[i][j].x+intemp1[i][j].w;
				temp1.rx=intemp1[i][j+1].x;
				r[i].r.push_back(temp1);		
			}
		}
		if((intemp1[i][intemp1[i].size()-1].x+intemp1[i][intemp1[i].size()-1].w)!=rightx){
			temp1.lx=intemp1[i][intemp1[i].size()-1].x+intemp1[i][intemp1[i].size()-1].w;
			temp1.rx=rightx;
			r[i].r.push_back(temp1);
		}
	}
}
int check_region(node cell){
	for(int i=0;i<r[cell.row_num].r.size();i++){
		if(cell.x<=r[cell.row_num].r[i].rx && cell.x>=r[cell.row_num].r[i].lx && cell.x+cell.w>=r[cell.row_num].r[i].lx && cell.x+cell.w<=r[cell.row_num].r[i].rx){
			return i;	
		} 
	}	
	return -1;
}
//void addcell(){
//	
//}
//void addcluster(){
//	
//}
//void collapse(){
//	
//}
//void placerow(int row_num,node cell){
//	double pos;
//	double ec,wc,qc;
//	vector <range>::iterator itx;
//	if(r[row_num].c.size()==0){
//		cluster new_cluster;
//		pos=check_region(cell);//true:可以擺 false:不能擺 
//		if(pos!=-1){
//			new_cluster.row_num=row_num;
//			new_cluster.x=cell.x;
//			new_cluster.w=cell.w;
//			new_cluster.n.push_back(cell);//把cell放進cluster 
//			r[row_num].c.push_back(new_cluster);//把cluster放進row的data structure 
//			vector <range> r_new;
//			range temp;
//			temp.lx=r[row_num].r[pos].lx;
//			temp.rx=new_cluster.x;
//			r_new.push_back(temp);
//			temp.lx=new_cluster.x+new_cluster.w;
//			temp.rx=r[row_num].r[pos].rx;
//			r_new.push_back(temp);
//			itx=r[row_num].r.begin();
//			r[row_num].r.insert(itx+pos+1,r_new.begin(),r_new.end());
//			r[row_num].r.erase(itx+pos);
//		}
//	}
//	if(r[row_num].c.size()!=0){
//		cluster last;
//		last=r[row_num].c[r[row_num].c.size()-1];
//		qc=0;
//		wc=0;
//		ec=0;
//	}
//	
//	else{
//		
//		
//				
//	}
//}
double cost(node cell){
	double cost=0;
	for(int i=0;i<o_cellarray[cell.index].net.size();i++){
		it=netarray[i].node.begin();
		int left_bound_x=x[*it].x;
		int low_bound_y=x[*it].y;
		int right_bound_x=x[*it].x+x[*it].w;
		int high_bound_y=x[*it].y+x[*it].h;
		for(it=netarray[i].node.begin();it!=netarray[i].node.end();it++){
			int lx=x[(*it)].x;
			int ly=x[(*it)].y;
			int rx=x[(*it)].x+x[(*it)].w;
			int hy=x[(*it)].y+x[(*it)].h;
			if(lx<left_bound_x)
				left_bound_x=lx;
			if(rx>right_bound_x)
				right_bound_x=rx;
			if(ly<low_bound_y)
				low_bound_y=ly;
			if(hy> high_bound_y)	
				 high_bound_y=hy;
		}
		for(it=netarray[i].ninode.begin();it!=netarray[i].ninode.end();it++){
			int lx=y[(*it)].x;
			int ly=y[(*it)].y;
			int rx=y[(*it)].x+y[(*it)].w;
			int hy=y[(*it)].y+y[(*it)].h;
			if(lx<left_bound_x)
				left_bound_x=lx;
			if(rx>right_bound_x)
				right_bound_x=rx;
			if(ly<low_bound_y)
				low_bound_y=ly;
			if(hy> high_bound_y)	
				 high_bound_y=hy;
		}
		cost=cost+(right_bound_x-left_bound_x)+(high_bound_y-low_bound_y);
	}
	return cost;
}
void abacus(){
	for(int i=0;i<bound;i++){
		x[i].row_num=(x[i].y-lowy)/placeheight;
		stand.push_back(x[i]);
	}//對每個stand cell做編碼 
	sort(stand.begin(),stand.end(),xn_sort);	
	double c_best=0,c=0;
	int r_best=0;
	node save_node;
	row save_row;
	int t=5;
	int s=16;
	vector <range>::iterator itx;
	for(int i=0;i<stand.size();i++){
		c_best=(rightx-leftx)+(uppery-lowy);
		r_best=0;
		
		save_node=stand[i];
		for(int j=stand[i].row_num-3;j<stand[i].row_num+3;j++){
			if(j>=r.size() || j<0)
				continue;
			save_row=r[j];
			stand[i].row_num=j;
			int k=check_region(stand[i]);
			int o=0;
			if (k==-1){
				continue;
			}
//			while(k==-1){
//				stand[i].x=stand[i].x+1;
//				k=check_region(stand[i]);
//				o++;
//			}
			c=cost(stand[i]);	
			if(c<c_best){
				c_best=c;
				r_best=j;
			}
				r[j]=save_row;
				stand[i]=save_node;
			}
			//stand[i].y=stand[i].y+(r_best-stand[i].row_num)*placeheight;
			
				stand[i].x=stand[i].x+i-stand[i].row_num;
				stand[i].row_num=r_best;
				int p=check_region(stand[i]);
			vector <range> r_new;
			range temp;
			temp.lx=r[r_best].r[p].lx;
			temp.rx=stand[i].x;
			r[r_best].r.push_back(temp);
			temp.lx=stand[i].x+stand[i].w;
			temp.rx=r[r_best].r[p].rx;
			r[r_best].r.push_back(temp);
//			r[r_best].r[p].lx=-1;
//			r[r_best].r[p].rx=-1;
//			itx=r[r_best].r.begin();
//			r[r_best].r.insert(itx+p,r_new.begin(),r_new.end());
//			r[r_best].r.erase(itx+p);
					
		}
		for(int i=0;i<stand.size();i++){
			if(stand[i].index==t){
				stand[i].x=stand[i].x+s;
			}
		}
	}	
	bool d(node x1,node y1){
		stringstream ss,ss1;
		int n,n1;
		ss<<x1.name.erase(0,1);
		ss>>n;
		ss1<<y1.name.erase(0,1);
		ss1>>n1;
	return n<n1;
			
	}
int main(){
//	clock_t start_time,end_time;
//  double run_time=0;
//	start_time=clock();
	fstream fo;
	fo.open("simple.pl",ios::out);
	readnode("simple.nodes");
	o_cellarray.resize(x.size());
	p_cellarray.resize(y.size());
	readnet("simple.nets");
	readlegal("simple.legal");
	readshape("simple.shapes");
	readscl("simple.scl");
	cut_range();
	abacus();
	cout<<"x_size:"<<x.size();
	sort(stand.begin(),stand.end(),d);
	for(int i=0;i<stand.size();i++){
		fo<<stand[i].name<<"\t"<<stand[i].x<<"\t"<<stand[i].y<<"\t:"<<"\tN"<<endl;
	}
	for(int i=bound;i<x.size();i++){
		fo<<x[i].name<<"\t"<<x[i].x<<"\t"<<x[i].y<<"\t:"<<"\tN"<<"\t"<<"/FIXED"<<endl;
	}
	for(int i=0;i<y.size();i++){
		fo<<y[i].name<<"\t"<<y[i].x<<"\t"<<y[i].y<<"\t:"<<"\tN"<<"\t"<<"/FIXED_NI"<<endl;
	}
}
