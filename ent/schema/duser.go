package schema

import (
	"entgo.io/ent"
	"entgo.io/ent/schema/field"
)

// DUser holds the schema definition for the DUser entity.
type DUser struct {
	ent.Schema
}

// Fields of the DUser.
func (DUser) Fields() []ent.Field {
	return []ent.Field{
		field.String("id").NotEmpty(),
		field.String("name").NotEmpty().Unique(),
		field.String("password_hash").NotEmpty(),
	}
}

// Edges of the DUser.
func (DUser) Edges() []ent.Edge {
	return nil
}
