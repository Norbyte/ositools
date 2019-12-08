#include "stdafx.h"
#include "OsirisProxy.h"

namespace osidbg
{
	IdentityAdapterMap::IdentityAdapterMap(OsirisStaticGlobals const & globals)
		: globals_(globals)
	{}

	void IdentityAdapterMap::UpdateAdapters()
	{
		if ((*globals_.Adapters) == nullptr) {
			DEBUG("Not syncing adapters - DB not available");
			return;
		}

		auto const & adapterDb = (*globals_.Adapters)->Db;
		for (unsigned i = 0; i < adapterDb.Size; i++) {
			auto adapter = adapterDb.Start[i];
			TryAddAdapter(adapter);
		}
	}

	bool IdentityAdapterMap::HasAllAdapters()
	{
		for (unsigned i = 0; i <= MaxColumns; i++) {
			auto it = adapters_.find(i);
			if (it == adapters_.end()) return false;
		}

		return true;
	}

	Adapter * IdentityAdapterMap::FindAdapter(uint8_t columns)
	{
		auto it = adapters_.find(columns);
		if (it == adapters_.end()) return nullptr;
		return it->second;
	}

	void IdentityAdapterMap::TryAddAdapter(Adapter * adapter)
	{
		if (adapter->Constants.Data.Items.Size > 0) return;

		auto varMap = adapter->ColumnToVarMaps.Start;
		auto varEnd = adapter->ColumnToVarMaps.Start + adapter->VarToColumnMapCount;
		int8_t varIdx = 0;
		while (varMap < varEnd) {
			if (varIdx++ != *varMap++) return;
		}

		adapters_[(uint8_t)adapter->VarToColumnMapCount] = adapter;
	}
}
