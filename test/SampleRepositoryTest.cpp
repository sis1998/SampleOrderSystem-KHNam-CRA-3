#include <gtest/gtest.h>
#include "../src/repository/SampleRepository.h"
#include "../src/model/SampleModel.h"
#include <cstdio>

TEST(SampleRepositoryTest, SaveAndLoad_Roundtrip) {
    SampleModel m;
    m.add({"S-001", "Silicon", 0.5, 0.92, 100});
    SampleRepository repo;
    repo.save(m, "test_samples_tmp.json");
    SampleModel m2;
    repo.load(m2, "test_samples_tmp.json");
    EXPECT_EQ(m2.findById("S-001")->stock, 100);
    EXPECT_EQ(m2.findById("S-001")->name, "Silicon");
    // 정리
    std::remove("test_samples_tmp.json");
}

TEST(SampleRepositoryTest, Load_FileNotExists_NoOp) {
    SampleModel m;
    SampleRepository repo;
    repo.load(m, "nonexistent_file.json"); // 예외 없이 통과
    EXPECT_TRUE(m.getAll().empty());
}
