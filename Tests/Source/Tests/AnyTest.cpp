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

        IT("can compare wrapped values for equality")
        {
            REQUIRE(any(14) == any(static_cast<int64>(14)));
            REQUIRE(any(0) == any(false));
            REQUIRE(any(14.2) != any(14.1));
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

        IT("can compare enums for equality")
        {
            REQUIRE(anyEnum == any(TestEnumFoo));
            REQUIRE(anyEnum != any(TestEnumBar));
            REQUIRE(anyEnumClass == any(TestEnumClass::Bar));
            REQUIRE(anyEnumClass != any(TestEnumClass::Foo));
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

        IT("can compare wrapped values for equality")
        {
            REQUIRE(any(String("Hello, this is a test.")) == anyString);
            REQUIRE(any(String("Hello, this is different.")) != anyString);
            REQUIRE(anyPoint == any(Point<int>(4, 15)));
            REQUIRE(anyPoint != any(Point<int>(4, -15)));
        }

        IT("does not perform implicit conversion when checking for equality")
        {
            REQUIRE(anyPoint != any(Point<int64>(4, 15)));
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

    CONTEXT("polymorphic types")
    {
        // Define Animal class with subclasses Dog and Cat
        class LivingBeing
        {};
        class Animal : public LivingBeing
        {
        public:
            Animal(int weight)
            : weight(weight) {}

            bool operator==(const Animal& other) const
            {
                return (weight == other.weight);
            }

            int weight;
        };

        class Dog : public Animal
        {
        public:
            Dog(int weight, int barksPerMinute)
            : Animal(weight), barksPerMinute(barksPerMinute) {}

            bool operator==(const Dog& other) const
            {
                return (Animal::operator==(other) && barksPerMinute == other.barksPerMinute);
            }

            int barksPerMinute;
        };

        class Cat : public Animal
        {
        public:
            Cat(int weight, double cutenessFactor)
            : Animal(weight), cutenessFactor(cutenessFactor) {}

            bool operator==(const Cat& other) const
            {
                return (Animal::operator==(other) && cutenessFactor == other.cutenessFactor);
            }

            double cutenessFactor;
        };

        // Create instances
        any sameWeightAsGarfield(Animal(120));
        any bello(Dog(16, 9));
        any garfield(Cat(120, 0.001));
        any identicalGarfield(Cat(120, 0.001));
        any cuteGarfield(Cat(120, 1));
        any helloKitty(Cat(16, 10000));

        IT("can compare instances of the same subclass")
        {
            REQUIRE(garfield == garfield);
            REQUIRE_FALSE(garfield != garfield);
            REQUIRE(garfield == identicalGarfield);
            REQUIRE_FALSE(garfield != identicalGarfield);

            REQUIRE_FALSE(garfield == cuteGarfield);
            REQUIRE_FALSE(cuteGarfield == helloKitty);
        }

        IT("can compare instances of separate subclasses")
        {
            // Should be non-equal, although the Animal part is equal
            REQUIRE_FALSE(bello == helloKitty);
        }

        IT("is equal even if the wrapped value has been copied")
        {
            any copy(garfield.get<Cat>());

            REQUIRE(copy == garfield);
            REQUIRE(identicalGarfield == copy);
        }
    }

    CONTEXT("Equality with pointer comparison")
    {
        struct Foo
        {
            Foo(int x)
            : x(x) {}

            int x;
        };

        IT("is equal if the wrapped value is the same instance")
        {
            any anyFoo(Foo(5));
            CHECK(anyFoo == anyFoo);

            any another = anyFoo;

            REQUIRE(anyFoo == another);
        }

        IT("is non-equal if two instances are constructed from the same value")
        {
            Foo foo(16);
            REQUIRE(any(foo) != any(foo));
        }

        IT("is non-equal if two instances are constructed from two different values")
        {
            Foo foo1(16);
            Foo foo2(16);

            REQUIRE(any(foo1) != any(foo2));
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
