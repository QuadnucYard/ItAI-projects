#include "decision_tree.hpp"
#include "utils.hpp"

int main() {

	using namespace qy::ai;
	fs::current_path(fs::absolute(__FILE__).parent_path());
	Dataset train_ds("data/train0.csv", true);
	Dataset test_ds("data/test5.csv", true);

	DecisionTree dt(train_ds);
	// print_tree(dt);
	auto ans = dt.classify(test_ds.examples);
	auto gt = get_labels(test_ds.examples);
	std::cout << "Precision: " << evaluate(ans, gt) << std::endl;
	// print_examples(train_ds.examples);
	return 0;
}