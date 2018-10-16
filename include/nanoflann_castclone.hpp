#pragma once


 template <class D, class SND>
 void castcopytree(D*d, typename D::Node* & dn, const  SND* sn)
 {
	using DT = typename D::DistanceType;
	if(sn->child1 != 0 && sn->child2 != 0) // is_leaf
	{
		auto *mn = d->pool.template allocate<typename D::Node>();
		mn->node_type.sub.divfeat = sn->node_type.sub.divfeat;
		mn->node_type.sub.divlow = DT(sn->node_type.sub.divlow); // casting
		mn->node_type.sub.divhigh = DT(sn->node_type.sub.divhigh); // casting
		if(sn->child1)
			castcopytree(d, mn->child1, sn->child1);
		else
			mn->child1 = 0;
		if(sn->child2)
			castcopytree(d, mn->child2, sn->child2);
		else
			mn->child2 = 0;
		dn = mn;
	}
	else
	{
		int div = sn->node_type.sub.divfeat;
		auto *ln = d->pool.template allocate<typename D::Node >();
		ln->child1 = ln->child2 = NULL;
		ln->node_type.lr.left = sn->node_type.lr.left;
		ln->node_type.lr.right = sn->node_type.lr.right;
		dn = ln;
	}     	
 }

 template <class From, class To>
 void castcopyindex(const From * index_f, To * index)
 {
	typedef typename To::DistanceType DT;

	index->m_size = index_f->m_size;
	index->m_size_at_index_build = index->m_size;
	index->dim = index_f->dim;
	index->root_bbox.resize(index_f->dim);
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