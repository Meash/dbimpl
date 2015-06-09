#include <stdint-gcc.h>
#include <gtest/gtest.h>
#include "../../src/database/operators/Register.hpp"
#include "../../src/database/operators/PrintOperator.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

TEST(PrintOperatorTest, PrintSingleIntRegister) {
  Register reg(1337);
  std::vector<std::vector<Register *>> tableVector;
  std::vector<Register *> rowVector;
  rowVector.push_back(&reg);
  tableVector.push_back(rowVector);

  std::stringstream stream;
  PrintOperator<uint64_t> printOperator(tableVector, stream);

  ASSERT_THROW(printOperator.next(), std::invalid_argument);
  printOperator.open();
  ASSERT_TRUE(printOperator.next());
  ASSERT_EQ(1, printOperator.getOutput().size());
  ASSERT_EQ(1337, (printOperator.getOutput()[0])->getValue());
  ASSERT_EQ("1337", stream.str());
}

TEST(PrintOperatorTest, PrintCharRegister) {
  Register reg("Hallo Welt");
  std::vector<std::vector<Register *>> tableVector;
  std::vector<Register *> rowVector;
  rowVector.push_back(&reg);
  tableVector.push_back(rowVector);

  std::stringstream stream;
  PrintOperator<const char *> printOperator(tableVector, stream);

  ASSERT_THROW(printOperator.next(), std::invalid_argument);
  printOperator.open();
  ASSERT_TRUE(printOperator.next());
  ASSERT_EQ(1, printOperator.getOutput().size());
  ASSERT_EQ("Hallo Welt", (printOperator.getOutput()[0])->getValue());
  ASSERT_EQ("Hallo Welt", stream.str());
}
