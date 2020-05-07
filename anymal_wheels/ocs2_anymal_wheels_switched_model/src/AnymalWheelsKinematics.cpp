/*
 * AnymalWheelsKinematics.cpp
 *
 *  Created on: Nov 25, 2019
 *      Author: Marko Bjelonic
 */

#include "ocs2_anymal_wheels_switched_model/core/AnymalWheelsKinematics.h"
#include <ocs2_anymal_wheels_switched_model/core/WheelsSwitchedModel.h>

#include <iit/rbd/traits/TraitSelector.h>

#include "ocs2_anymal_wheels_switched_model/generated/jacobians.h"
#include "ocs2_anymal_wheels_switched_model/generated/transforms.h"

namespace anymal {
namespace tpl {

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
AnymalWheelsKinematics<SCALAR_T>* AnymalWheelsKinematics<SCALAR_T>::clone() const {
  return new AnymalWheelsKinematics<SCALAR_T>(*this);
}

template <typename SCALAR_T>
switched_model::vector3_s_t<SCALAR_T> AnymalWheelsKinematics<SCALAR_T>::positionBaseToWheelAxisInBaseFrame(
    size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  using trait_t = typename iit::rbd::tpl::TraitSelector<SCALAR_T>::Trait;

  const auto q = getExtendedJointCoordinates(jointPositions);

  switch (footIndex) {
    case LF: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_LF_WHEEL_L p;
      return p(q).template topRightCorner<3, 1>();
    }
    case RF: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_RF_WHEEL_L p;
      return p(q).template topRightCorner<3, 1>();
    }
    case LH: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_LH_WHEEL_L p;
      return p(q).template topRightCorner<3, 1>();
    }
    case RH: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_RH_WHEEL_L p;
      return p(q).template topRightCorner<3, 1>();
    }
    default:
      std::runtime_error("Undefined endeffector index.");
      break;
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
switched_model::vector3_s_t<SCALAR_T> AnymalWheelsKinematics<SCALAR_T>::positionBaseToFootInBaseFrame(
    size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const {

  SCALAR_T haa = jointPositions(footIndex * 3);  // HAA angle for the requested leg
  switched_model::vector3_s_t<SCALAR_T> wheelOffset;
  wheelOffset.x() = 0;
  wheelOffset.y() = wheelRadius_ * sin(haa);
  wheelOffset.z() = -wheelRadius_ * cos(haa);

  return positionBaseToWheelAxisInBaseFrame(footIndex, jointPositions) + wheelOffset;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
template <typename SCALAR_T>
typename AnymalWheelsKinematics<SCALAR_T>::joint_jacobian_t AnymalWheelsKinematics<SCALAR_T>::baseToFootJacobianInBaseFrame(
    size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  using trait_t = typename iit::rbd::tpl::TraitSelector<SCALAR_T>::Trait;

  joint_jacobian_t footJacobian;
  footJacobian.setZero();

  Eigen::Matrix<SCALAR_T, 6, 3> wheelOffsetJacobian;
  wheelOffsetJacobian.setZero();

  SCALAR_T haa = jointPositions(footIndex * 3);  // HAA angle for the requested leg
  wheelOffsetJacobian(4, 0) = wheelRadius_ * cos(haa);
  wheelOffsetJacobian(5, 0) = wheelRadius_ * sin(haa);

  const auto q = getExtendedJointCoordinates(jointPositions);

  switch (footIndex) {
    case LF: {
      typename iit::ANYmal::tpl::Jacobians<trait_t>::Type_fr_base_J_fr_LF_WHEEL_L fr_base_J_fr_LF_foot_;
      footJacobian.template block<6, 3>(0, 0) = fr_base_J_fr_LF_foot_(q).template leftCols<3>() + wheelOffsetJacobian;
      break;
    }
    case RF: {
      typename iit::ANYmal::tpl::Jacobians<trait_t>::Type_fr_base_J_fr_RF_WHEEL_L fr_base_J_fr_RF_foot_;
      footJacobian.template block<6, 3>(0, 3) = fr_base_J_fr_RF_foot_(q).template leftCols<3>() + wheelOffsetJacobian;
      break;
    }
    case LH: {
      typename iit::ANYmal::tpl::Jacobians<trait_t>::Type_fr_base_J_fr_LH_WHEEL_L fr_base_J_fr_LH_foot_;
      footJacobian.template block<6, 3>(0, 6) = fr_base_J_fr_LH_foot_(q).template leftCols<3>() + wheelOffsetJacobian;
      break;
    }
    case RH: {
      typename iit::ANYmal::tpl::Jacobians<trait_t>::Type_fr_base_J_fr_RH_WHEEL_L fr_base_J_fr_RH_foot_;
      footJacobian.template block<6, 3>(0, 9) = fr_base_J_fr_RH_foot_(q).template leftCols<3>() + wheelOffsetJacobian;
      break;
    }
    default: {
      std::runtime_error("Undefined endeffector index.");
      break;
    }
  }

  return footJacobian;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/

template <typename SCALAR_T>
switched_model::matrix3_s_t<SCALAR_T> AnymalWheelsKinematics<SCALAR_T>::wheelAxisOrientationInBaseFrame(
    size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  using trait_t = typename iit::rbd::tpl::TraitSelector<SCALAR_T>::Trait;

  const auto q = getExtendedJointCoordinates(jointPositions);
  switch (footIndex) {
    case LF: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_LF_WHEEL_L fr_base_X_fr_LF_WHEEL_L;
      return fr_base_X_fr_LF_WHEEL_L(q).template topLeftCorner<3, 3>();
             }
    case RF: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_RF_WHEEL_L fr_base_X_fr_RF_WHEEL_L;
      return fr_base_X_fr_RF_WHEEL_L(q).template topLeftCorner<3, 3>();
             }
    case LH: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_LH_WHEEL_L fr_base_X_fr_LH_WHEEL_L;
      return fr_base_X_fr_LH_WHEEL_L(q).template topLeftCorner<3, 3>();
             }
    case RH: {
      typename iit::ANYmal::tpl::HomogeneousTransforms<trait_t>::Type_fr_base_X_fr_RH_WHEEL_L fr_base_X_fr_RH_WHEEL_L;
      return fr_base_X_fr_RH_WHEEL_L(q).template topLeftCorner<3, 3>();
             }
    default:
             std::runtime_error("Undefined endeffector index.");
             break;
  }
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/

template <typename SCALAR_T>
switched_model::matrix3_s_t<SCALAR_T> AnymalWheelsKinematics<SCALAR_T>::footOrientationInBaseFrame(
    size_t footIndex, const switched_model::joint_coordinate_s_t<SCALAR_T>& jointPositions) const {
  using trait_t = typename iit::rbd::tpl::TraitSelector<SCALAR_T>::Trait;

  // clang-format off
  switched_model::matrix3_s_t<SCALAR_T> wheelAxis_R_foot;
  wheelAxis_R_foot<< SCALAR_T(1), SCALAR_T(0), SCALAR_T(0),
                       SCALAR_T(0), SCALAR_T(0), SCALAR_T(-1),
                       SCALAR_T(0), SCALAR_T(1), SCALAR_T(0);
  // clang-format on

  return wheelAxisOrientationInBaseFrame(footIndex, jointPositions) * wheelAxis_R_foot;
}

}  // namespace tpl
}  // end of namespace anymal

// Explicit instantiation
template class anymal::tpl::AnymalWheelsKinematics<double>;
template class anymal::tpl::AnymalWheelsKinematics<ocs2::CppAdInterface<double>::ad_scalar_t>;
