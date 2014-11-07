#include "SpanItem.h"
SpanItem::SpanItem(double prob){
	this->prob = prob;
}


SpanItem::SpanItem(int i, int k, int j, int type,
			   int dir, int comp,
			   double prob, FVLink &fvlink,
			   SpanItem *pleft, SpanItem *pright) {
	this->Init(i, type, dir, prob);
	
	this->r = k;
	this->t = j;
	this->comp = comp;
	length = 6;
	//this->fvlink = fvlink;
	//this->fvlink.Add(fvlink);
	this->pleft = pleft;
	this->pright = pright;
		
}

SpanItem::SpanItem(int s, int type, int dir, double prob){
	Init(s, type, dir, prob);
}

void SpanItem::Init(int s, int type, int dir, double prob) {
    this->s = s;
	this->r = s;
	this->t = s;
    this->dir = dir;
	this->comp = -1;
    this->type = type;
    length = 2;
    this->prob = prob;
	//this->fvlink.pfvs.clear();
	this->pleft = NULL;
	this->pright = NULL;

}

SpanItem::SpanItem(int s, int type, int dir, double prob, FVLink &fvlink) {

	this->Init(s, type, dir, prob);
	//this->fvlink.Add(fvlink);
	//this->fvlink = fvlink;
}

SpanItem::SpanItem() {}

void SpanItem::CopyValuesTo(SpanItem &p) {
	p.s = s;
	p.r = r;
	p.t = t;
	p.dir = dir;
	p.comp = comp;
	p.prob = prob;
	//p.fvlink = fvlink;
	p.length = length;
	//p.next = next;
	p.pleft = pleft;
	p.pright = pright;
	p.type = type;
}
void SpanItem::Reset(int s, int type, int dir, double prob){
    this->Init(s, type, dir, prob);
    //this->fvlink.pfvs.clear();
    //this->next.clear();
}

void SpanItem::Reset(int s, int type, int dir, double prob, FVLink &fvlink){
	this->Init(s, type, dir, prob);	
	//this->fvlink.pfvs.clear();
	//this->fvlink = fvlink;
	//this->next.clear();
} 

void SpanItem::Reset(int i, int k, int j, int type,
               int dir, int comp,
               double prob, FVLink &fvlink,
               SpanItem *pleft, SpanItem *pright){
    this->Init(i, type, dir, prob);

    this->r = k;
    this->t = j;
    this->comp = comp;
    length = 6;
	//this->fvlink = fvlink;
	//this->next.clear();
	this->pleft = pleft;
	this->pright = pright;
    //this->next.push_back(leftsi);
    //this->next.push_back(rightsi);
} 
void SpanItem::Reset(int i,  int type,
               double prob, FVLink &fvlink,
               SpanItem *pleft){
    this->Init(i, type, -1, prob);

    this->r = -1;
    this->t = type;
    this->comp = -1;
    length = 6;
    //this->fvlink = fvlink;
    //this->next.clear();
    this->pleft = pleft;
    //this->next.push_back(leftsi);
    //this->next.push_back(rightsi);
}
	
bool SpanItem::Equals(SpanItem &p) {
	return s == p.s && t == p.t && r == p.r
	    && dir == p.dir && comp == p.comp
	    && type == p.type;
}

bool SpanItem::IsPre() { return length == 2; }
