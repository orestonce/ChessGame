// Code generated by entc, DO NOT EDIT.

package ent

import (
	"context"
	"fmt"
	"sync"

	"github.com/orestonce/ChessGame/ent/duser"
	"github.com/orestonce/ChessGame/ent/predicate"

	"entgo.io/ent"
)

const (
	// Operation types.
	OpCreate    = ent.OpCreate
	OpDelete    = ent.OpDelete
	OpDeleteOne = ent.OpDeleteOne
	OpUpdate    = ent.OpUpdate
	OpUpdateOne = ent.OpUpdateOne

	// Node types.
	TypeDUser = "DUser"
)

// DUserMutation represents an operation that mutates the DUser nodes in the graph.
type DUserMutation struct {
	config
	op            Op
	typ           string
	id            *string
	name          *string
	password_hash *string
	clearedFields map[string]struct{}
	done          bool
	oldValue      func(context.Context) (*DUser, error)
	predicates    []predicate.DUser
}

var _ ent.Mutation = (*DUserMutation)(nil)

// duserOption allows management of the mutation configuration using functional options.
type duserOption func(*DUserMutation)

// newDUserMutation creates new mutation for the DUser entity.
func newDUserMutation(c config, op Op, opts ...duserOption) *DUserMutation {
	m := &DUserMutation{
		config:        c,
		op:            op,
		typ:           TypeDUser,
		clearedFields: make(map[string]struct{}),
	}
	for _, opt := range opts {
		opt(m)
	}
	return m
}

// withDUserID sets the ID field of the mutation.
func withDUserID(id string) duserOption {
	return func(m *DUserMutation) {
		var (
			err   error
			once  sync.Once
			value *DUser
		)
		m.oldValue = func(ctx context.Context) (*DUser, error) {
			once.Do(func() {
				if m.done {
					err = fmt.Errorf("querying old values post mutation is not allowed")
				} else {
					value, err = m.Client().DUser.Get(ctx, id)
				}
			})
			return value, err
		}
		m.id = &id
	}
}

// withDUser sets the old DUser of the mutation.
func withDUser(node *DUser) duserOption {
	return func(m *DUserMutation) {
		m.oldValue = func(context.Context) (*DUser, error) {
			return node, nil
		}
		m.id = &node.ID
	}
}

// Client returns a new `ent.Client` from the mutation. If the mutation was
// executed in a transaction (ent.Tx), a transactional client is returned.
func (m DUserMutation) Client() *Client {
	client := &Client{config: m.config}
	client.init()
	return client
}

// Tx returns an `ent.Tx` for mutations that were executed in transactions;
// it returns an error otherwise.
func (m DUserMutation) Tx() (*Tx, error) {
	if _, ok := m.driver.(*txDriver); !ok {
		return nil, fmt.Errorf("ent: mutation is not running in a transaction")
	}
	tx := &Tx{config: m.config}
	tx.init()
	return tx, nil
}

// SetID sets the value of the id field. Note that this
// operation is only accepted on creation of DUser entities.
func (m *DUserMutation) SetID(id string) {
	m.id = &id
}

// ID returns the ID value in the mutation. Note that the ID is only available
// if it was provided to the builder or after it was returned from the database.
func (m *DUserMutation) ID() (id string, exists bool) {
	if m.id == nil {
		return
	}
	return *m.id, true
}

// SetName sets the "name" field.
func (m *DUserMutation) SetName(s string) {
	m.name = &s
}

// Name returns the value of the "name" field in the mutation.
func (m *DUserMutation) Name() (r string, exists bool) {
	v := m.name
	if v == nil {
		return
	}
	return *v, true
}

// OldName returns the old "name" field's value of the DUser entity.
// If the DUser object wasn't provided to the builder, the object is fetched from the database.
// An error is returned if the mutation operation is not UpdateOne, or the database query fails.
func (m *DUserMutation) OldName(ctx context.Context) (v string, err error) {
	if !m.op.Is(OpUpdateOne) {
		return v, fmt.Errorf("OldName is only allowed on UpdateOne operations")
	}
	if m.id == nil || m.oldValue == nil {
		return v, fmt.Errorf("OldName requires an ID field in the mutation")
	}
	oldValue, err := m.oldValue(ctx)
	if err != nil {
		return v, fmt.Errorf("querying old value for OldName: %w", err)
	}
	return oldValue.Name, nil
}

// ResetName resets all changes to the "name" field.
func (m *DUserMutation) ResetName() {
	m.name = nil
}

// SetPasswordHash sets the "password_hash" field.
func (m *DUserMutation) SetPasswordHash(s string) {
	m.password_hash = &s
}

// PasswordHash returns the value of the "password_hash" field in the mutation.
func (m *DUserMutation) PasswordHash() (r string, exists bool) {
	v := m.password_hash
	if v == nil {
		return
	}
	return *v, true
}

// OldPasswordHash returns the old "password_hash" field's value of the DUser entity.
// If the DUser object wasn't provided to the builder, the object is fetched from the database.
// An error is returned if the mutation operation is not UpdateOne, or the database query fails.
func (m *DUserMutation) OldPasswordHash(ctx context.Context) (v string, err error) {
	if !m.op.Is(OpUpdateOne) {
		return v, fmt.Errorf("OldPasswordHash is only allowed on UpdateOne operations")
	}
	if m.id == nil || m.oldValue == nil {
		return v, fmt.Errorf("OldPasswordHash requires an ID field in the mutation")
	}
	oldValue, err := m.oldValue(ctx)
	if err != nil {
		return v, fmt.Errorf("querying old value for OldPasswordHash: %w", err)
	}
	return oldValue.PasswordHash, nil
}

// ResetPasswordHash resets all changes to the "password_hash" field.
func (m *DUserMutation) ResetPasswordHash() {
	m.password_hash = nil
}

// Where appends a list predicates to the DUserMutation builder.
func (m *DUserMutation) Where(ps ...predicate.DUser) {
	m.predicates = append(m.predicates, ps...)
}

// Op returns the operation name.
func (m *DUserMutation) Op() Op {
	return m.op
}

// Type returns the node type of this mutation (DUser).
func (m *DUserMutation) Type() string {
	return m.typ
}

// Fields returns all fields that were changed during this mutation. Note that in
// order to get all numeric fields that were incremented/decremented, call
// AddedFields().
func (m *DUserMutation) Fields() []string {
	fields := make([]string, 0, 2)
	if m.name != nil {
		fields = append(fields, duser.FieldName)
	}
	if m.password_hash != nil {
		fields = append(fields, duser.FieldPasswordHash)
	}
	return fields
}

// Field returns the value of a field with the given name. The second boolean
// return value indicates that this field was not set, or was not defined in the
// schema.
func (m *DUserMutation) Field(name string) (ent.Value, bool) {
	switch name {
	case duser.FieldName:
		return m.Name()
	case duser.FieldPasswordHash:
		return m.PasswordHash()
	}
	return nil, false
}

// OldField returns the old value of the field from the database. An error is
// returned if the mutation operation is not UpdateOne, or the query to the
// database failed.
func (m *DUserMutation) OldField(ctx context.Context, name string) (ent.Value, error) {
	switch name {
	case duser.FieldName:
		return m.OldName(ctx)
	case duser.FieldPasswordHash:
		return m.OldPasswordHash(ctx)
	}
	return nil, fmt.Errorf("unknown DUser field %s", name)
}

// SetField sets the value of a field with the given name. It returns an error if
// the field is not defined in the schema, or if the type mismatched the field
// type.
func (m *DUserMutation) SetField(name string, value ent.Value) error {
	switch name {
	case duser.FieldName:
		v, ok := value.(string)
		if !ok {
			return fmt.Errorf("unexpected type %T for field %s", value, name)
		}
		m.SetName(v)
		return nil
	case duser.FieldPasswordHash:
		v, ok := value.(string)
		if !ok {
			return fmt.Errorf("unexpected type %T for field %s", value, name)
		}
		m.SetPasswordHash(v)
		return nil
	}
	return fmt.Errorf("unknown DUser field %s", name)
}

// AddedFields returns all numeric fields that were incremented/decremented during
// this mutation.
func (m *DUserMutation) AddedFields() []string {
	return nil
}

// AddedField returns the numeric value that was incremented/decremented on a field
// with the given name. The second boolean return value indicates that this field
// was not set, or was not defined in the schema.
func (m *DUserMutation) AddedField(name string) (ent.Value, bool) {
	return nil, false
}

// AddField adds the value to the field with the given name. It returns an error if
// the field is not defined in the schema, or if the type mismatched the field
// type.
func (m *DUserMutation) AddField(name string, value ent.Value) error {
	switch name {
	}
	return fmt.Errorf("unknown DUser numeric field %s", name)
}

// ClearedFields returns all nullable fields that were cleared during this
// mutation.
func (m *DUserMutation) ClearedFields() []string {
	return nil
}

// FieldCleared returns a boolean indicating if a field with the given name was
// cleared in this mutation.
func (m *DUserMutation) FieldCleared(name string) bool {
	_, ok := m.clearedFields[name]
	return ok
}

// ClearField clears the value of the field with the given name. It returns an
// error if the field is not defined in the schema.
func (m *DUserMutation) ClearField(name string) error {
	return fmt.Errorf("unknown DUser nullable field %s", name)
}

// ResetField resets all changes in the mutation for the field with the given name.
// It returns an error if the field is not defined in the schema.
func (m *DUserMutation) ResetField(name string) error {
	switch name {
	case duser.FieldName:
		m.ResetName()
		return nil
	case duser.FieldPasswordHash:
		m.ResetPasswordHash()
		return nil
	}
	return fmt.Errorf("unknown DUser field %s", name)
}

// AddedEdges returns all edge names that were set/added in this mutation.
func (m *DUserMutation) AddedEdges() []string {
	edges := make([]string, 0, 0)
	return edges
}

// AddedIDs returns all IDs (to other nodes) that were added for the given edge
// name in this mutation.
func (m *DUserMutation) AddedIDs(name string) []ent.Value {
	return nil
}

// RemovedEdges returns all edge names that were removed in this mutation.
func (m *DUserMutation) RemovedEdges() []string {
	edges := make([]string, 0, 0)
	return edges
}

// RemovedIDs returns all IDs (to other nodes) that were removed for the edge with
// the given name in this mutation.
func (m *DUserMutation) RemovedIDs(name string) []ent.Value {
	return nil
}

// ClearedEdges returns all edge names that were cleared in this mutation.
func (m *DUserMutation) ClearedEdges() []string {
	edges := make([]string, 0, 0)
	return edges
}

// EdgeCleared returns a boolean which indicates if the edge with the given name
// was cleared in this mutation.
func (m *DUserMutation) EdgeCleared(name string) bool {
	return false
}

// ClearEdge clears the value of the edge with the given name. It returns an error
// if that edge is not defined in the schema.
func (m *DUserMutation) ClearEdge(name string) error {
	return fmt.Errorf("unknown DUser unique edge %s", name)
}

// ResetEdge resets all changes to the edge with the given name in this mutation.
// It returns an error if the edge is not defined in the schema.
func (m *DUserMutation) ResetEdge(name string) error {
	return fmt.Errorf("unknown DUser edge %s", name)
}
