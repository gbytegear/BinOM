#ifndef RESOURCE_CONTROLL_TEST_HXX
#define RESOURCE_CONTROLL_TEST_HXX

#include "test/tester.hxx"

#include "libbinom/include/utils/resource_controll.hxx"

using namespace binom;
using namespace binom::priv;

void testResourceControll() {
  SEPARATOR
  TEST_ANOUNCE(Resorce controll test)
  PRINT_RUN(Link* hl_1_ptr = new Link(Resource{.type = VarType::ui64, .data = ui64(256)});)
  GRP(
    LOG("Test hard links")
    GRP(
      LOG("Created shared resource with first hard link")
      TEST(hl_1_ptr->getHardLinkCount() == 1)
      PRINT_RUN({ /* Create 3 hardlinks */)
        PRINT_RUN(Link hl_1(*hl_1_ptr, LinkType::hard_link);)
        PRINT_RUN(Link hl_2(hl_1, LinkType::hard_link);)
        PRINT_RUN(Link hl_3(hl_2, LinkType::hard_link);)
        LOG("Created 3 hardlinks")
        TEST(hl_1_ptr->getHardLinkCount() == 4)
      PRINT_RUN(} /* Destroy 3 hardlinks */)
      LOG("Destroy 3 hardlinks")
      TEST(hl_1_ptr->getHardLinkCount() == 1)
    )

    LOG("Test soft links")
    GRP(
      LOG("Create 3 softlinks")
      PRINT_RUN(Link sl_1(*hl_1_ptr, LinkType::soft_link);)
      PRINT_RUN(Link sl_2(sl_1, LinkType::soft_link);)
      PRINT_RUN(Link sl_3(sl_2, LinkType::soft_link);)
      TEST(sl_1.isResourceLinked())
      TEST(sl_2.isResourceLinked())
      TEST(sl_3.isResourceLinked())

      LOG("Destry last hardlink before destroing 3 softlinks")
      PRINT_RUN(delete hl_1_ptr;)

      TEST(!sl_1.isResourceLinked())
      TEST(!sl_2.isResourceLinked())
      TEST(!sl_3.isResourceLinked())
    )
  )

}

#endif // RESOURCE_CONTROLL_TEST_HXX
