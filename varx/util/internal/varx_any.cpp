namespace detail {
any::any(int value)
: type(Type::Int),
  value({ .intValue = value })
{}

any::any(int64 value)
: type(Type::Int64),
  value({ .int64Value = value })
{}

any::any(bool value)
: type(Type::Bool),
  value({ .boolValue = value })
{}

any::any(float value)
: type(Type::Float),
  value({ .floatValue = value })
{}

any::any(double value)
: type(Type::Double),
  value({ .doubleValue = value })
{}

bool any::equals(const any& other) const
{
    switch (type) {
        case Type::Int:
            return (other.isNumeric() && get<int>() == other.get<int>());
        case Type::Int64:
            return (other.isNumeric() && get<int64>() == other.get<int64>());
        case Type::Bool:
            return (other.type == Type::Bool && get<bool>() == other.get<bool>());
        case Type::Float:
            return (other.isNumeric() && get<float>() == other.get<float>());
        case Type::Double:
            return (other.isNumeric() && get<double>() == other.get<double>());
        case Type::Object:
            return (other.type == Type::Object && objectValue->equals(*other.objectValue));
    }
}

bool any::isNumeric() const
{
    return (type != Type::Bool && type != Type::Object);
}

any::Object::Object(const std::type_info& typeInfo)
: typeInfo(typeInfo)
{}

any::Object::~Object() {}
}
