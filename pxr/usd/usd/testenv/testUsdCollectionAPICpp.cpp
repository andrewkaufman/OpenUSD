//
// Copyright 2026 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//

#include "pxr/pxr.h"

#include "pxr/usd/sdf/path.h"
#include "pxr/usd/sdf/pathExpression.h"
#include "pxr/usd/usd/collectionAPI.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/stage.h"

#include "pxr/base/tf/token.h"

#include <cstdio>
#include <string>

PXR_NAMESPACE_USING_DIRECTIVE

// Tests of UsdCollectionAPI behavior that the Python bindings cannot reach.
// See testUsdCollectionAPI.py for most collection test cases.

static void
TestValidateExpressionCycle()
{
    printf("TestValidateExpressionCycle...\n");

    const UsdStageRefPtr stage = UsdStage::CreateInMemory();
    const UsdPrim prim = stage->DefinePrim(SdfPath("/Sets"));

    const UsdCollectionAPI a = UsdCollectionAPI::Apply(prim, TfToken("a"));
    const UsdCollectionAPI b = UsdCollectionAPI::Apply(prim, TfToken("b"));
    TF_AXIOM(a.CreateMembershipExpressionAttr().Set(
                 SdfPathExpression("/World %:b")));
    TF_AXIOM(b.CreateMembershipExpressionAttr().Set(
                 SdfPathExpression("/Other")));

    std::string reason;
    TF_AXIOM(a.Validate(&reason));
    TF_AXIOM(reason.empty());
    TF_AXIOM(a.Validate(nullptr));

    // Make b reference a, forming a cycle.  Resolving it emits warnings.
    TF_AXIOM(b.GetMembershipExpressionAttr().Set(
                 SdfPathExpression("/Other %:a")));

    TF_AXIOM(!a.Validate(&reason));
    TF_AXIOM(!reason.empty());
    TF_AXIOM(!a.Validate(nullptr));
}

int main()
{
    TestValidateExpressionCycle();

    printf("\n\n>>> Test SUCCEEDED\n");
    return 0;
}
