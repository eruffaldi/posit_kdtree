#pragma once


 template <class D, class SND>
 void castcopytree(D*d, typename D::Node* & dn, const  SND* sn)
 {
	using DT = typename D::DistanceType;
	dn = d->pool.template allocate<typename D::Node >();
	dn->child1 = dn->child2 = 0;

	if(sn->child1 != 0 || sn->child2 != 0) // is_leaf
	{
		dn->node_type.sub.divfeat = sn->node_type.sub.divfeat;
		dn->node_type.sub.divlow = DT(sn->node_type.sub.divlow); // casting
		dn->node_type.sub.divhigh = DT(sn->node_type.sub.divhigh); // casting
		if(sn->child1)
			castcopytree(d, dn->child1, sn->child1);
		if(sn->child2)
			castcopytree(d, dn->child2, sn->child2);
	}
	else
	{
		dn->node_type.lr.left = sn->node_type.lr.left;
		dn->node_type.lr.right = sn->node_type.lr.right;
	}     	
 }

 template <class From, class To>
 void castcopyindex(const From * index_f, To * index)
 {
	typedef typename To::DistanceType DT;

	index->m_size = index_f->m_size;
	index->m_size_at_index_build = index_f->m_size_at_index_build;
	index->dim = index_f->dim;
	resize(index->root_bbox,index->dim);
	const auto & sb = index_f->root_bbox ;
	auto & db = index->root_bbox ;
	for(int i = 0; i < index->dim; i++)
	{
		db[i].low = DT(sb[i].low); // cast
		db[i].high = DT(sb[i].high); // cast
	}
	index->m_leaf_max_size = index_f->m_leaf_max_size;
	index->vind = index_f->vind;
	castcopytree(index,index->root_node,index_f->root_node);
 }

/*
 template <class From, class To>
 void castcopyindexs(const From * index_f, To * index)
 {
	typedef typename To::DistanceType DT;

	index->m_size = index_f->m_size;
	index->m_size_at_index_build = index->m_size;
	index->dim = index_f->dim;
	const auto & sb = index_f->root_bbox ;
	auto & db = index->root_bbox ;
	for(int i = 0; i < index->dim; i++)
	{
		db[i].low = DT(sb[i].low); // cast
		db[i].high = DT(sb[i].high); // cast
	}
	index->m_leaf_max_size = index_f->m_leaf_max_size;
	index->vind = index_f->vind;
	castcopytree(index,index->root_node,index_f->root_node);
 }
 */