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

#include "cgp.h"
#include "functions.h"

#include <core/darwin.h>
#include <core/properties.h>
#include <core/stringify.h>

#include <array>
#include <cstdint>
#include <utility>
#include <vector>
using namespace std;

namespace cgp {

class Population;

enum class MutationStrategy {
  FixedCount,
  Probabilistic,
};

inline auto customStringify(core::TypeTag<MutationStrategy>) {
  static auto stringify = new core::StringifyKnownValues<MutationStrategy>{
    { MutationStrategy::FixedCount, "fixed_count" },
    { MutationStrategy::Probabilistic, "probabilistic" },
  };
  return stringify;
}

struct FixedCountMutation : public core::PropertySet {
  PROPERTY(mutation_count, int, 2, "Number of mutations per genotype");
};

struct ProbabilisticMutation : public core::PropertySet {
  PROPERTY(connection_mutation_chance,
           float,
           0.05f,
           "Probability of mutating a connection");
  PROPERTY(function_mutation_chance,
           float,
           0.05f,
           "Probability of mutating a node's function");
  PROPERTY(output_mutation_chance,
           float,
           0.1f,
           "Probability of mutating an output gene");
  PROPERTY(constant_mutation_chance,
           float,
           0.1f,
           "Probability of mutating an evolvable constant");
};

struct MutationVariant : public core::PropertySetVariant<MutationStrategy> {
  CASE(MutationStrategy::FixedCount, fixed_count, FixedCountMutation);
  CASE(MutationStrategy::Probabilistic, probabilistic, ProbabilisticMutation);
};

using IndexType = uint16_t;

struct FunctionGene {
  FunctionId function;
  array<IndexType, kMaxFunctionArity> connections;

  friend void to_json(json& json_obj, const FunctionGene& gene);
  friend void from_json(const json& json_obj, FunctionGene& gene);
  friend bool operator==(const FunctionGene& a, const FunctionGene& b);
};

struct OutputGene {
  IndexType connection;

  friend void to_json(json& json_obj, const OutputGene& gene);
  friend void from_json(const json& json_obj, OutputGene& gene);
  friend bool operator==(const OutputGene& a, const OutputGene& b);
};

class Genotype : public darwin::Genotype {
 public:
  explicit Genotype(const Population* population);

  unique_ptr<darwin::Brain> grow() const override;
  unique_ptr<darwin::Genotype> clone() const override;

  json save() const override;
  void load(const json& json_obj) override;
  void reset() override;

  void createPrimordialSeed();
  void probabilisticMutation(const ProbabilisticMutation& config);
  void fixedCountMutation(const FixedCountMutation& config);
  void inherit(const Genotype& parent1, const Genotype& parent2, float preference);

  const Population* population() const { return population_; }
  const vector<FunctionGene>& functionGenes() const { return function_genes_; }
  const vector<OutputGene>& outputGenes() const { return output_genes_; }
  
  float getEvolvableConstant(int function_id) const;

  friend bool operator==(const Genotype& a, const Genotype& b);

 private:
  template <class PRED>
  void mutationHelper(PRED& predicates);

  pair<IndexType, IndexType> connectionRange(int layer, int levels_back) const;

 private:
  const Population* population_ = nullptr;

  vector<FunctionGene> function_genes_;
  vector<OutputGene> output_genes_;
  vector<float> constants_genes_;
};

}  // namespace cgp
