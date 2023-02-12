#pragma once

#include "Delegate.h"

namespace DoDo {
#define DECLARE_DELEGATE(DelegateName) FUNC_DECLARE_DELEGATE(DelegateName, void)

#define DECLARE_DELEGATE_RetVal(ReturnValueType, DelegateName) FUNC_DECLARE_DELEGATE(DelegateName, ReturnValueType)

#define DECLARE_DELEGATE_OneParam(DelegateName, Param1Type) FUNC_DECLARE_DELEGATE(DelegateName, void, Param1Type)
#define DECLARE_DELEGATE_RetVal_OneParam(ReturnValueType, DelegateName, Param1Type) FUNC_DECLARE_DELEGATE(DelegateName, ReturnValueType, Param1Type)

#define DECLARE_DELEGATE_TwoParams(DelegateName, Param1Type, Param2Type) FUNC_DECLARE_DELEGATE(DelegateName, ReturnValueType, Param1Type, Param2Type)
#define DECLARE_DELEGATE_RetVal_TwoParams(ReturnValueType, DelegateName, Param1Type, Param2Type) FUNC_DECLARE_DELEGATE(DelegateName, ReturnValueType, Param1Type, Param2Type)
}