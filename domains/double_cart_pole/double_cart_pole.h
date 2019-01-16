// Copyright 2019 The Darwin Neuroevolution Framework Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <core/darwin.h>
#include <core/properties.h>

namespace double_cart_pole {

//! Double-Cart-Pole domain configuration
struct Config : public core::PropertySet {
  PROPERTY(gravity, float, 9.8f, "Gravitational acceleration");
  PROPERTY(max_distance, float, 2.4f, "Maximum distance from the center");
  PROPERTY(max_angle, float, 60.0f, "Maximum angle from vertical");
  PROPERTY(max_initial_angle, float, 10.0f, "Maximum starting angle from vertical");
  PROPERTY(pole_1_length, float, 1.5f, "First pole length");
  PROPERTY(pole_1_density, float, 1.0f, "First pole density");
  PROPERTY(pole_2_length, float, 0.3f, "Second pole length");
  PROPERTY(pole_2_density, float, 1.0f, "Second pole density");
  PROPERTY(cart_density, float, 0.0f, "Cart density");
  PROPERTY(cart_friction, float, 0.0f, "Cart friction");
  PROPERTY(max_force, float, 10.0f, "Maximum force which can be applied to the cart");
  
  PROPERTY(input_pole_angle, bool, true, "Use the pole angle as input");
  PROPERTY(input_angular_velocity, bool, false, "Use the angular velocity as input");
  PROPERTY(input_cart_distance, bool, true, "Use the cart distance as input");
  PROPERTY(input_cart_velocity, bool, false, "Use the cart velocity as input");
  
  PROPERTY(test_worlds, int, 5, "Number of test worlds per generation");
  PROPERTY(max_steps, int, 1000, "Maximum number of steps per episode");

  PROPERTY(discrete_controls,
           bool,
           false,
           "Force the actuator force to fixed +/-discrete_force_magnitude");

  PROPERTY(discrete_force_magnitude,
           float,
           2.5f,
           "The fixed force magnitude used if discrete_controls is true");
};

//! Domain: Double-Cart-Pole
//!
//! A variation of the cart-pole domain (cart_pole::CartPole), with two
//! independent poles attached to the cart.
//!
//! ![](images/double_cart_pole_sandbox.png)
//!
//! The cart starts in the middle (x = 0) and the initial pole angles is a random value in
//! `[-max_initial_angle, +max_initial_angle]` range. An episode is successful if both
//! poles remain between `-max_angle` and `+max_angle` for at least `max_steps`. The cart
//! position must also stay between [-max_distance, +max_distance].
//!
//! ### Inputs
//!
//! The inputs are configurable by individually selecting at least one of:
//! - pole_angle(1,2) (from vertical)
//! - angular_velocity(1,2)
//! - cart_distance (from the center)
//! - cart_velocity
//!
//! Input | Value
//! -----:|------
//!   0,1 | pole_angle(1,2)
//!   2,3 | angular_velocity(1,2)
//!     4 | cart_distance
//!     5 | cart_velocity
//!
//! ### Outputs
//!
//! The single output indicates the horizontal force to be applied to the cart. This can
//! be discrete (fixed +/-discrete_force_magnitude depending on the sign of the output) or
//! can be continuous (the output value maps directly to the force magnitude)
//!
//! Output | Value
//! ------:|------
//!      0 | force
//!
class DoubleCartPole : public darwin::Domain {
 public:
  explicit DoubleCartPole(const core::PropertySet& config);

  size_t inputs() const override;
  size_t outputs() const override;

  bool evaluatePopulation(darwin::Population* population) const override;
  
  const Config& config() const { return config_; }
  
  float randomInitialAngle() const;
  
 private:
  void validateConfiguration();

 private:
  Config config_;
};

class Factory : public darwin::DomainFactory {
  unique_ptr<darwin::Domain> create(const core::PropertySet& config) override;
  unique_ptr<core::PropertySet> defaultConfig(darwin::ComplexityHint hint) const override;
};

inline void init() {
  darwin::registry()->domains.add<Factory>("double_cart_pole");
}

}  // namespace double_cart_pole
