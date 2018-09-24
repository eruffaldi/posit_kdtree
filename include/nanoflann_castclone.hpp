#pragma once


 template <class D, class DN, class SN>
 void castcopytree(D*d, DN&dn, const SN&sn)
 {
	using DT = typename D::DistanceType;
	if(sn->child1 != 0 && sn->child2 != 0) // is_leaf
	{
		const auto & smn = *sn;
		auto *mn = d->pool.template allocate<typename D::Node>();
		mn->node_type.sub.divfeat = smn.node_type.sub.divfeat;
		mn->node_type.sub.divlow = DT(smn.node_type.sub.divlow); // casting
		mn->node_type.sub.divhigh = DT(smn.node_type.sub.divhigh); // casting
		dn = mn;
		if(mn->child1)
			castcopytree(d, mn->child1, smn.child1);
		else
			mn->child1 = 0;
		if(mn->child2)
			castcopytree(d, mn->child2, smn.child2);
		else
			mn->child2 = 0;
	}
	else
	{
		int div = sn->node_type.sub.divfeat;
		const auto & sln = *sn;
		auto *ln = d->pool.template allocate<typename D::Node >();
		dn = ln;
		ln->node_type.lr.left = sln.node_type.lr.left;
		ln->node_type.lr.right = sln.node_type.lr.right;
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