#include "../Other/TestPrefix.h"

using Catch::Contains;
using detail::any;

TEST_CASE("any",
          "[any]")
{
    CONTEXT("Primitive types")
    {
        any anyInt(17);
        any anyInt64(static_cast<int64>(34));
        any anyBool(true);
        any anyFloat(51.68f);
        any anyDouble(85.102);

        IT("has the correct stored type")
        {
            REQUIRE(anyInt.is<int>());
            REQUIRE(anyInt64.is<int64>());
            REQUIRE(anyBool.is<bool>());
            REQUIRE(anyFloat.is<float>());
            REQUIRE(anyDouble.is<double>());
        }

        IT("has the correct stored value")
        {
            REQUIRE(anyInt.get<int>() == 17);
            REQUIRE(anyInt64.get<int64>() == 34);
            REQUIRE(anyBool.get<bool>() == true);
            REQUIRE(anyFloat.get<float>() == 51.68f);
            REQUIRE(anyDouble.get<double>() == 85.102);
        }

        IT("coerces between primitive types")
        {
            REQUIRE(anyInt.get<bool>() == true);
            REQUIRE(anyBool.get<int64>() == 1);
            REQUIRE(anyInt64.get<double>() == 34);
            REQUIRE(anyFloat.get<int>() == 51);
        }

        IT("throws when trying to extract an object value")
        {
            REQUIRE_THROWS_WITH(anyInt.get<String>(), Contains("Error getting type from any."));
        }
    }

    CONTEXT("Enums")
    {
        enum TestEnum { TestEnumFoo = 17,
                        TestEnumBar };
        enum class TestEnumClass { Foo,
                                   Bar };
        any anyEnum(TestEnumFoo);
        any anyEnumClass(TestEnumClass::Bar);

        IT("can be initialized from enums")
        {
            REQUIRE(anyEnum.is<TestEnum>());
            REQUIRE(anyEnumClass.is<TestEnumClass>());
        }

        IT("stores the enum value correctly")
        {
            REQUIRE(anyEnum.get<TestEnum>() == TestEnumFoo);
            REQUIRE(anyEnumClass.get<TestEnumClass>() == TestEnumClass::Bar);
        }

        IT("does not coerce enums to int")
        {
            REQUIRE_THROWS_WITH(anyEnum.get<juce::int64>(), Contains("Error getting type from any."));
        }
    }

    CONTEXT("non-scalar types")
    {
        any anyString(String("Hello, this is a test."));
        any anyPoint(Point<int>(4, 15));

        IT("has the correct stored type")
        {
            REQUIRE(anyString.is<String>());
            REQUIRE(anyPoint.is<Point<int>>());

            REQUIRE_FALSE(anyString.is<Point<int>>());
            REQUIRE_FALSE(anyPoint.is<Point<int64>>());
            REQUIRE_FALSE(anyPoint.is<String>());
        }

        IT("stores the value correctly")
        {
            REQUIRE(anyString.get<String>() == "Hello, this is a test.");
            REQUIRE(anyPoint.get<Point<int>>() == Point<int>(4, 15));
        }

        IT("throws an assertion when trying to retrieve an unrelated type")
        {
            REQUIRE_THROWS_WITH(anyString.get<int>(), Contains("Error getting type from any."));
            REQUIRE_THROWS_WITH(anyString.get<Point<int>>(), Contains("Error getting type from any."));
            REQUIRE_THROWS_WITH(anyString.get<Point<float>>(), Contains("Error getting type from any."));
        }

        IT("holds an independent copy of the original value")
        {
            // Create point, wrap it in any
            Point<int> p(14, 66);
            any wrapped(p);

            // Change original
            p.x = 53;

            // wrapped should not be changed
            REQUIRE(wrapped.get<Point<int>>() == Point<int>(14, 66));
        }
    }

    CONTEXT("pointers")
    {
        IT("can store a pointer to a struct")
        {
            struct S
            {
                int i;
            };
            
            S s;
            s.i = 1829;
            
            any anyS(&s);
            REQUIRE(anyS.get<S*>()->i == 1829);
        }
        
        IT("can store a pointer to an int")
        {
            int x = -19381;
            any anyInt(&x);
            
            REQUIRE(*anyInt.get<int*>() == -19381);
        }
    }

    CONTEXT("Copying and moving the wrapped type")
    {
        CopyAndMoveConstructible::Counters counters;

        IT("Uses copy-construction for lvalues")
        {
            CopyAndMoveConstructible value(&counters);
            any test(value);

            REQUIRE(counters.numCopyConstructions == 1);
            REQUIRE(counters.numMoveConstructions == 0);
            REQUIRE(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveAssignments == 0);
        }

        IT("Uses move-construction for rvalues")
        {
            // Create instance, using an rvalue
            any test((CopyAndMoveConstructible(&counters)));

            REQUIRE(counters.numCopyConstructions == 0);
            REQUIRE(counters.numMoveConstructions == 1);
            REQUIRE(counters.numCopyAssignments == 0);
            REQUIRE(counters.numMoveAssignments == 0);

            IT("Shares the wrapped value between instances")
            {
                // Create another any instance, using copy constructor
                any another(test);

                // Wrapped value should not be copied
                REQUIRE(counters.numCopyConstructions == 0);
                REQUIRE(counters.numMoveConstructions == 1);
                REQUIRE(counters.numCopyAssignments == 0);
                REQUIRE(counters.numMoveAssignments == 0);
            }
            
            IT("doesn't copy when getting the object value")
            {
                // Should return by const reference
                const CopyAndMoveConstructible& ref = test.get<CopyAndMoveConstructible>();
                
                // Wrapped value should not be copied
                REQUIRE(ref.counters->numCopyConstructions == 0);
                REQUIRE(ref.counters->numMoveConstructions == 1);
                REQUIRE(ref.counters->numCopyAssignments == 0);
                REQUIRE(ref.counters->numMoveAssignments == 0);
            }
        }
    }
    
    CONTEXT("move-only type")
    {
        IT("can hold a move-only (non-copyable) type")
        {
            // Create a unique_ptr
            auto ptr = std::unique_ptr<int>(new int());
            *ptr = 17;
            int* const address = ptr.get();
            
            // Construct using rvalue
            any anyPtr(std::move(ptr));
            
            // Get by const reference
            const std::unique_ptr<int>& ptrRef = anyPtr.get<std::unique_ptr<int>>();
            
            // Check address and value
            REQUIRE(ptrRef.get() == address);
            REQUIRE(*ptrRef == 17);
        }
    }
}
