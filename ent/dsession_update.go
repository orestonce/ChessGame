// Code generated by entc, DO NOT EDIT.

package ent

import (
	"context"
	"fmt"
	"time"

	"entgo.io/ent/dialect/sql"
	"entgo.io/ent/dialect/sql/sqlgraph"
	"entgo.io/ent/schema/field"
	"github.com/orestonce/ChessGame/ent/dsession"
	"github.com/orestonce/ChessGame/ent/predicate"
)

// DSessionUpdate is the builder for updating DSession entities.
type DSessionUpdate struct {
	config
	hooks    []Hook
	mutation *DSessionMutation
}

// Where appends a list predicates to the DSessionUpdate builder.
func (du *DSessionUpdate) Where(ps ...predicate.DSession) *DSessionUpdate {
	du.mutation.Where(ps...)
	return du
}

// SetUserID sets the "user_id" field.
func (du *DSessionUpdate) SetUserID(s string) *DSessionUpdate {
	du.mutation.SetUserID(s)
	return du
}

// SetNillableUserID sets the "user_id" field if the given value is not nil.
func (du *DSessionUpdate) SetNillableUserID(s *string) *DSessionUpdate {
	if s != nil {
		du.SetUserID(*s)
	}
	return du
}

// SetRoomID sets the "room_id" field.
func (du *DSessionUpdate) SetRoomID(s string) *DSessionUpdate {
	du.mutation.SetRoomID(s)
	return du
}

// SetNillableRoomID sets the "room_id" field if the given value is not nil.
func (du *DSessionUpdate) SetNillableRoomID(s *string) *DSessionUpdate {
	if s != nil {
		du.SetRoomID(*s)
	}
	return du
}

// SetCreateTime sets the "create_time" field.
func (du *DSessionUpdate) SetCreateTime(t time.Time) *DSessionUpdate {
	du.mutation.SetCreateTime(t)
	return du
}

// Mutation returns the DSessionMutation object of the builder.
func (du *DSessionUpdate) Mutation() *DSessionMutation {
	return du.mutation
}

// Save executes the query and returns the number of nodes affected by the update operation.
func (du *DSessionUpdate) Save(ctx context.Context) (int, error) {
	var (
		err      error
		affected int
	)
	if len(du.hooks) == 0 {
		affected, err = du.sqlSave(ctx)
	} else {
		var mut Mutator = MutateFunc(func(ctx context.Context, m Mutation) (Value, error) {
			mutation, ok := m.(*DSessionMutation)
			if !ok {
				return nil, fmt.Errorf("unexpected mutation type %T", m)
			}
			du.mutation = mutation
			affected, err = du.sqlSave(ctx)
			mutation.done = true
			return affected, err
		})
		for i := len(du.hooks) - 1; i >= 0; i-- {
			if du.hooks[i] == nil {
				return 0, fmt.Errorf("ent: uninitialized hook (forgotten import ent/runtime?)")
			}
			mut = du.hooks[i](mut)
		}
		if _, err := mut.Mutate(ctx, du.mutation); err != nil {
			return 0, err
		}
	}
	return affected, err
}

// SaveX is like Save, but panics if an error occurs.
func (du *DSessionUpdate) SaveX(ctx context.Context) int {
	affected, err := du.Save(ctx)
	if err != nil {
		panic(err)
	}
	return affected
}

// Exec executes the query.
func (du *DSessionUpdate) Exec(ctx context.Context) error {
	_, err := du.Save(ctx)
	return err
}

// ExecX is like Exec, but panics if an error occurs.
func (du *DSessionUpdate) ExecX(ctx context.Context) {
	if err := du.Exec(ctx); err != nil {
		panic(err)
	}
}

func (du *DSessionUpdate) sqlSave(ctx context.Context) (n int, err error) {
	_spec := &sqlgraph.UpdateSpec{
		Node: &sqlgraph.NodeSpec{
			Table:   dsession.Table,
			Columns: dsession.Columns,
			ID: &sqlgraph.FieldSpec{
				Type:   field.TypeString,
				Column: dsession.FieldID,
			},
		},
	}
	if ps := du.mutation.predicates; len(ps) > 0 {
		_spec.Predicate = func(selector *sql.Selector) {
			for i := range ps {
				ps[i](selector)
			}
		}
	}
	if value, ok := du.mutation.UserID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dsession.FieldUserID,
		})
	}
	if value, ok := du.mutation.RoomID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dsession.FieldRoomID,
		})
	}
	if value, ok := du.mutation.CreateTime(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeTime,
			Value:  value,
			Column: dsession.FieldCreateTime,
		})
	}
	if n, err = sqlgraph.UpdateNodes(ctx, du.driver, _spec); err != nil {
		if _, ok := err.(*sqlgraph.NotFoundError); ok {
			err = &NotFoundError{dsession.Label}
		} else if sqlgraph.IsConstraintError(err) {
			err = &ConstraintError{err.Error(), err}
		}
		return 0, err
	}
	return n, nil
}

// DSessionUpdateOne is the builder for updating a single DSession entity.
type DSessionUpdateOne struct {
	config
	fields   []string
	hooks    []Hook
	mutation *DSessionMutation
}

// SetUserID sets the "user_id" field.
func (duo *DSessionUpdateOne) SetUserID(s string) *DSessionUpdateOne {
	duo.mutation.SetUserID(s)
	return duo
}

// SetNillableUserID sets the "user_id" field if the given value is not nil.
func (duo *DSessionUpdateOne) SetNillableUserID(s *string) *DSessionUpdateOne {
	if s != nil {
		duo.SetUserID(*s)
	}
	return duo
}

// SetRoomID sets the "room_id" field.
func (duo *DSessionUpdateOne) SetRoomID(s string) *DSessionUpdateOne {
	duo.mutation.SetRoomID(s)
	return duo
}

// SetNillableRoomID sets the "room_id" field if the given value is not nil.
func (duo *DSessionUpdateOne) SetNillableRoomID(s *string) *DSessionUpdateOne {
	if s != nil {
		duo.SetRoomID(*s)
	}
	return duo
}

// SetCreateTime sets the "create_time" field.
func (duo *DSessionUpdateOne) SetCreateTime(t time.Time) *DSessionUpdateOne {
	duo.mutation.SetCreateTime(t)
	return duo
}

// Mutation returns the DSessionMutation object of the builder.
func (duo *DSessionUpdateOne) Mutation() *DSessionMutation {
	return duo.mutation
}

// Select allows selecting one or more fields (columns) of the returned entity.
// The default is selecting all fields defined in the entity schema.
func (duo *DSessionUpdateOne) Select(field string, fields ...string) *DSessionUpdateOne {
	duo.fields = append([]string{field}, fields...)
	return duo
}

// Save executes the query and returns the updated DSession entity.
func (duo *DSessionUpdateOne) Save(ctx context.Context) (*DSession, error) {
	var (
		err  error
		node *DSession
	)
	if len(duo.hooks) == 0 {
		node, err = duo.sqlSave(ctx)
	} else {
		var mut Mutator = MutateFunc(func(ctx context.Context, m Mutation) (Value, error) {
			mutation, ok := m.(*DSessionMutation)
			if !ok {
				return nil, fmt.Errorf("unexpected mutation type %T", m)
			}
			duo.mutation = mutation
			node, err = duo.sqlSave(ctx)
			mutation.done = true
			return node, err
		})
		for i := len(duo.hooks) - 1; i >= 0; i-- {
			if duo.hooks[i] == nil {
				return nil, fmt.Errorf("ent: uninitialized hook (forgotten import ent/runtime?)")
			}
			mut = duo.hooks[i](mut)
		}
		if _, err := mut.Mutate(ctx, duo.mutation); err != nil {
			return nil, err
		}
	}
	return node, err
}

// SaveX is like Save, but panics if an error occurs.
func (duo *DSessionUpdateOne) SaveX(ctx context.Context) *DSession {
	node, err := duo.Save(ctx)
	if err != nil {
		panic(err)
	}
	return node
}

// Exec executes the query on the entity.
func (duo *DSessionUpdateOne) Exec(ctx context.Context) error {
	_, err := duo.Save(ctx)
	return err
}

// ExecX is like Exec, but panics if an error occurs.
func (duo *DSessionUpdateOne) ExecX(ctx context.Context) {
	if err := duo.Exec(ctx); err != nil {
		panic(err)
	}
}

func (duo *DSessionUpdateOne) sqlSave(ctx context.Context) (_node *DSession, err error) {
	_spec := &sqlgraph.UpdateSpec{
		Node: &sqlgraph.NodeSpec{
			Table:   dsession.Table,
			Columns: dsession.Columns,
			ID: &sqlgraph.FieldSpec{
				Type:   field.TypeString,
				Column: dsession.FieldID,
			},
		},
	}
	id, ok := duo.mutation.ID()
	if !ok {
		return nil, &ValidationError{Name: "ID", err: fmt.Errorf("missing DSession.ID for update")}
	}
	_spec.Node.ID.Value = id
	if fields := duo.fields; len(fields) > 0 {
		_spec.Node.Columns = make([]string, 0, len(fields))
		_spec.Node.Columns = append(_spec.Node.Columns, dsession.FieldID)
		for _, f := range fields {
			if !dsession.ValidColumn(f) {
				return nil, &ValidationError{Name: f, err: fmt.Errorf("ent: invalid field %q for query", f)}
			}
			if f != dsession.FieldID {
				_spec.Node.Columns = append(_spec.Node.Columns, f)
			}
		}
	}
	if ps := duo.mutation.predicates; len(ps) > 0 {
		_spec.Predicate = func(selector *sql.Selector) {
			for i := range ps {
				ps[i](selector)
			}
		}
	}
	if value, ok := duo.mutation.UserID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dsession.FieldUserID,
		})
	}
	if value, ok := duo.mutation.RoomID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dsession.FieldRoomID,
		})
	}
	if value, ok := duo.mutation.CreateTime(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeTime,
			Value:  value,
			Column: dsession.FieldCreateTime,
		})
	}
	_node = &DSession{config: duo.config}
	_spec.Assign = _node.assignValues
	_spec.ScanValues = _node.scanValues
	if err = sqlgraph.UpdateNode(ctx, duo.driver, _spec); err != nil {
		if _, ok := err.(*sqlgraph.NotFoundError); ok {
			err = &NotFoundError{dsession.Label}
		} else if sqlgraph.IsConstraintError(err) {
			err = &ConstraintError{err.Error(), err}
		}
		return nil, err
	}
	return _node, nil
}
