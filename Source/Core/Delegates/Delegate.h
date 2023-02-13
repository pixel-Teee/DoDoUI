#pragma once

#include "IDelegateInstance.h"

#include "DelegateBase.h"

#define FUNC_DECLARE_DELEGATE(DelegateName, ReturnType, ... ) \
	typedef TDelegate<ReturnType(__VA_ARGS__)> DelegateName;

#include "DelegateInstanceInterface.h"
#include "DelegateInstancesImpl.h"
#include "DelegateSignatureImpl.inl"
#include "DelegateCombinations.h"

