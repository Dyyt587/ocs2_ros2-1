/******************************************************************************
Copyright (c) 2020, Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

 * Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <gtest/gtest.h>

#include <ocs2_centroidal_model/example/anymal/AnymalKinoCentroidalDynamics.h>
#include <ocs2_centroidal_model/example/anymal/AnymalKinoCentroidalDynamicsAD.h>
#include <ocs2_centroidal_model/example/anymal/definitions.h>
#include <ocs2_centroidal_model/CentroidalModelPinocchioInterface.h>

#include <ocs2_pinocchio_interface/urdf.h>

using namespace ocs2;

using CentroidalModelType = CentroidalModelPinocchioInterface<scalar_t>::CentroidalModelType;
using CentroidalModelTypeAD = CentroidalModelPinocchioInterface<ad_scalar_t>::CentroidalModelType;

static CentroidalModelPinocchioInterface<scalar_t> getAnymalCentroidalModelInterface() {
  // build a joint model having just 2 joints: one translational joint and one spherical joint
  pinocchio::JointModelComposite jointComposite(2);
  jointComposite.addJoint(pinocchio::JointModelTranslation());
  jointComposite.addJoint(pinocchio::JointModelSphericalZYX());
  PinocchioInterface pinocchioInterface(getPinocchioInterfaceFromUrdfFile(anymalUrdfPath, jointComposite));
  const size_t nq = pinocchioInterface.getModel().nq;
  CentroidalModelPinocchioInterface<scalar_t> anymalCentroidalModelInterface(CentroidalModelType::SingleRigidBodyDynamics,
                                                                           anymalInitialState.tail(nq), anymal3DofContactNames,
                                                                           anymal6DofContactNames, pinocchioInterface);
  return anymalCentroidalModelInterface;
}

static CentroidalModelPinocchioInterface<ad_scalar_t> getAnymalCentroidalModelInterfaceAD() {
  // build a joint model having just 2 joints: one translational joint and one spherical joint
  pinocchio::JointModelComposite jointComposite(2);
  jointComposite.addJoint(pinocchio::JointModelTranslation());
  jointComposite.addJoint(pinocchio::JointModelSphericalZYX());
  PinocchioInterface pinocchioInterface(getPinocchioInterfaceFromUrdfFile(anymalUrdfPath, jointComposite));
  const size_t nq = pinocchioInterface.getModel().nq;
  CentroidalModelPinocchioInterface<ad_scalar_t> anymalCentroidalModelInterface(
          CentroidalModelTypeAD::SingleRigidBodyDynamics, anymalInitialState.tail(nq).cast<ad_scalar_t>(), anymal3DofContactNames,
          anymal6DofContactNames, pinocchioInterface.toCppAd());
  return anymalCentroidalModelInterface;
}

TEST(AnymalCentroidalModelTestInit, InitModelFromUrdf) {
  auto anymalCentroidalModelInterface = getAnymalCentroidalModelInterface();
  auto& model = anymalCentroidalModelInterface.getRobotModel();
  auto& data = anymalCentroidalModelInterface.getRobotData();
  auto& centroidalModelInfo = anymalCentroidalModelInterface.getCentroidalModelInfo();

  std::cerr << "nq " << model.nq << '\n';
  std::cerr << "nv " << model.nv << '\n';
  std::cerr << "STATE_DIM " << anymal::STATE_DIM << '\n';
  std::cerr << "INPUT_DIM " << anymal::INPUT_DIM << '\n';

  EXPECT_EQ(model.nq + 6, anymal::STATE_DIM);
  EXPECT_EQ(model.nq - 6 + 3 * centroidalModelInfo.numThreeDofContacts + 6 * centroidalModelInfo.numSixDofContacts, anymal::INPUT_DIM);
}

TEST(AnymalCentroidalModelTestInit, InitModelFromUrdfAD) {
  auto anymalCentroidalModelInterfaceAD = getAnymalCentroidalModelInterfaceAD();
  auto& model = anymalCentroidalModelInterfaceAD.getRobotModel();
  auto& data = anymalCentroidalModelInterfaceAD.getRobotData();
  auto& centroidalModelInfo = anymalCentroidalModelInterfaceAD.getCentroidalModelInfo();

  std::cerr << "nq " << model.nq << '\n';
  std::cerr << "nv " << model.nv << '\n';
  std::cerr << "STATE_DIM " << anymal::STATE_DIM << '\n';
  std::cerr << "INPUT_DIM " << anymal::INPUT_DIM << '\n';

  EXPECT_EQ(model.nq + 6, anymal::STATE_DIM);
  EXPECT_EQ(model.nq - 6 + 3 * centroidalModelInfo.numThreeDofContacts + 6 * centroidalModelInfo.numSixDofContacts, anymal::INPUT_DIM);
}

class AnymalCentroidalModelTest : public testing::Test {
public:
  AnymalCentroidalModelTest() {
    AnymalKinoCentroidalDynamicsPtr = std::make_shared<AnymalKinoCentroidalDynamics>(getAnymalCentroidalModelInterface());
    AnymalKinoCentroidalDynamicsAdPtr = std::make_shared<AnymalKinoCentroidalDynamicsAD>(getAnymalCentroidalModelInterfaceAD());

    srand(0);
    time = 0.0;
    state = ocs2::vector_t::Random(anymal::STATE_DIM);
    input = ocs2::vector_t::Random(anymal::INPUT_DIM);
  }

  ocs2::scalar_t time;
  ocs2::vector_t state;
  ocs2::vector_t input;

  std::shared_ptr<AnymalKinoCentroidalDynamics> AnymalKinoCentroidalDynamicsPtr;
  std::shared_ptr<AnymalKinoCentroidalDynamicsAD> AnymalKinoCentroidalDynamicsAdPtr;
};

static void visualMatrixCompare(const ocs2::matrix_t& A, const ocs2::matrix_t& B, double tol = 1e-6) {
  if (A.rows() != B.rows() || A.cols() != B.cols()) {
    std::cerr << "Matrices are not of same size\n";
  }
  for (int row = 0; row < A.rows(); row++) {
    for (int col = 0; col < A.cols(); col++) {
      const double error = std::abs(A(row, col) - B(row, col));
      if (error < tol) {
        std::cerr << " ";
      } else {
        std::cerr << " (" << row << ", " << col << "): " << error;
      }
    }
    std::cerr << '\n';
  }
}

static void compareApproximation(const ocs2::VectorFunctionLinearApproximation& a, const ocs2::VectorFunctionLinearApproximation& b,
                                 double tol = 1e-6) {
  if (!a.f.isApprox(b.f)) {
    std::cerr << "compare dynamics\n";
    visualMatrixCompare(a.f.transpose(), b.f.transpose(), tol);
  }
  if (!a.dfdx.isApprox(b.dfdx)) {
    std::cerr << "compare dfdx\n";
    visualMatrixCompare(a.dfdx, b.dfdx, tol);
  }
  if (!a.dfdu.isApprox(b.dfdu)) {
    std::cerr << "compare dfdu\n";
    visualMatrixCompare(a.dfdu, b.dfdu, tol);
  }

  EXPECT_TRUE(a.f.isApprox(b.f));
  // EXPECT_TRUE(a.dfdx.isApprox(b.dfdx));
  // The gradients of the base pose time derivatives are not correct due to a wrong dh_dq_ in pinocchio
  EXPECT_TRUE(a.dfdx.topRows<6>().isApprox(b.dfdx.topRows<6>()));
  EXPECT_TRUE(a.dfdx.bottomRows<12>().isApprox(b.dfdx.bottomRows<12>()));
  EXPECT_TRUE(a.dfdx.leftCols<9>().isApprox(b.dfdx.leftCols<9>()));
  EXPECT_TRUE(a.dfdx.rightCols<12>().isApprox(b.dfdx.rightCols<12>()));
  EXPECT_TRUE(a.dfdu.isApprox(b.dfdu));
}

TEST_F(AnymalCentroidalModelTest, ComputeFlowMap) {
  const auto dynamics = AnymalKinoCentroidalDynamicsPtr->computeFlowMap(time, state, input);
}

TEST_F(AnymalCentroidalModelTest, ComputeLinearApproximation) {
  const auto linearApproximation = AnymalKinoCentroidalDynamicsPtr->linearApproximation(time, state, input);
}

TEST_F(AnymalCentroidalModelTest, CompareFlowMaps) {
  const auto stateDerivative = AnymalKinoCentroidalDynamicsPtr->computeFlowMap(time, state, input);
  const auto stateDerivativeAd = AnymalKinoCentroidalDynamicsAdPtr->computeFlowMap(time, state, input);
  EXPECT_TRUE(stateDerivative.isApprox(stateDerivativeAd));
}

TEST_F(AnymalCentroidalModelTest, CompareFlowMapLinearApproximations) {
  const auto linearApproximation = AnymalKinoCentroidalDynamicsPtr->linearApproximation(time, state, input);
  const auto linearApproximationAd = AnymalKinoCentroidalDynamicsAdPtr->linearApproximation(time, state, input);
  compareApproximation(linearApproximation, linearApproximationAd);
}
