// Code generated by entc, DO NOT EDIT.

package ent

import (
	"context"
	"fmt"
	"time"

	"entgo.io/ent/dialect/sql"
	"entgo.io/ent/dialect/sql/sqlgraph"
	"entgo.io/ent/schema/field"
	"github.com/orestonce/ChessGame/ent/dchat"
	"github.com/orestonce/ChessGame/ent/predicate"
)

// DChatUpdate is the builder for updating DChat entities.
type DChatUpdate struct {
	config
	hooks    []Hook
	mutation *DChatMutation
}

// Where appends a list predicates to the DChatUpdate builder.
func (du *DChatUpdate) Where(ps ...predicate.DChat) *DChatUpdate {
	du.mutation.Where(ps...)
	return du
}

// SetSessionID sets the "session_id" field.
func (du *DChatUpdate) SetSessionID(s string) *DChatUpdate {
	du.mutation.SetSessionID(s)
	return du
}

// SetUserID sets the "user_id" field.
func (du *DChatUpdate) SetUserID(s string) *DChatUpdate {
	du.mutation.SetUserID(s)
	return du
}

// SetRoomID sets the "room_id" field.
func (du *DChatUpdate) SetRoomID(s string) *DChatUpdate {
	du.mutation.SetRoomID(s)
	return du
}

// SetText sets the "text" field.
func (du *DChatUpdate) SetText(s string) *DChatUpdate {
	du.mutation.SetText(s)
	return du
}

// SetCreateTime sets the "create_time" field.
func (du *DChatUpdate) SetCreateTime(t time.Time) *DChatUpdate {
	du.mutation.SetCreateTime(t)
	return du
}

// Mutation returns the DChatMutation object of the builder.
func (du *DChatUpdate) Mutation() *DChatMutation {
	return du.mutation
}

// Save executes the query and returns the number of nodes affected by the update operation.
func (du *DChatUpdate) Save(ctx context.Context) (int, error) {
	var (
		err      error
		affected int
	)
	if len(du.hooks) == 0 {
		affected, err = du.sqlSave(ctx)
	} else {
		var mut Mutator = MutateFunc(func(ctx context.Context, m Mutation) (Value, error) {
			mutation, ok := m.(*DChatMutation)
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
func (du *DChatUpdate) SaveX(ctx context.Context) int {
	affected, err := du.Save(ctx)
	if err != nil {
		panic(err)
	}
	return affected
}

// Exec executes the query.
func (du *DChatUpdate) Exec(ctx context.Context) error {
	_, err := du.Save(ctx)
	return err
}

// ExecX is like Exec, but panics if an error occurs.
func (du *DChatUpdate) ExecX(ctx context.Context) {
	if err := du.Exec(ctx); err != nil {
		panic(err)
	}
}

func (du *DChatUpdate) sqlSave(ctx context.Context) (n int, err error) {
	_spec := &sqlgraph.UpdateSpec{
		Node: &sqlgraph.NodeSpec{
			Table:   dchat.Table,
			Columns: dchat.Columns,
			ID: &sqlgraph.FieldSpec{
				Type:   field.TypeString,
				Column: dchat.FieldID,
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
	if value, ok := du.mutation.SessionID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldSessionID,
		})
	}
	if value, ok := du.mutation.UserID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldUserID,
		})
	}
	if value, ok := du.mutation.RoomID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldRoomID,
		})
	}
	if value, ok := du.mutation.Text(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldText,
		})
	}
	if value, ok := du.mutation.CreateTime(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeTime,
			Value:  value,
			Column: dchat.FieldCreateTime,
		})
	}
	if n, err = sqlgraph.UpdateNodes(ctx, du.driver, _spec); err != nil {
		if _, ok := err.(*sqlgraph.NotFoundError); ok {
			err = &NotFoundError{dchat.Label}
		} else if sqlgraph.IsConstraintError(err) {
			err = &ConstraintError{err.Error(), err}
		}
		return 0, err
	}
	return n, nil
}

// DChatUpdateOne is the builder for updating a single DChat entity.
type DChatUpdateOne struct {
	config
	fields   []string
	hooks    []Hook
	mutation *DChatMutation
}

// SetSessionID sets the "session_id" field.
func (duo *DChatUpdateOne) SetSessionID(s string) *DChatUpdateOne {
	duo.mutation.SetSessionID(s)
	return duo
}

// SetUserID sets the "user_id" field.
func (duo *DChatUpdateOne) SetUserID(s string) *DChatUpdateOne {
	duo.mutation.SetUserID(s)
	return duo
}

// SetRoomID sets the "room_id" field.
func (duo *DChatUpdateOne) SetRoomID(s string) *DChatUpdateOne {
	duo.mutation.SetRoomID(s)
	return duo
}

// SetText sets the "text" field.
func (duo *DChatUpdateOne) SetText(s string) *DChatUpdateOne {
	duo.mutation.SetText(s)
	return duo
}

// SetCreateTime sets the "create_time" field.
func (duo *DChatUpdateOne) SetCreateTime(t time.Time) *DChatUpdateOne {
	duo.mutation.SetCreateTime(t)
	return duo
}

// Mutation returns the DChatMutation object of the builder.
func (duo *DChatUpdateOne) Mutation() *DChatMutation {
	return duo.mutation
}

// Select allows selecting one or more fields (columns) of the returned entity.
// The default is selecting all fields defined in the entity schema.
func (duo *DChatUpdateOne) Select(field string, fields ...string) *DChatUpdateOne {
	duo.fields = append([]string{field}, fields...)
	return duo
}

// Save executes the query and returns the updated DChat entity.
func (duo *DChatUpdateOne) Save(ctx context.Context) (*DChat, error) {
	var (
		err  error
		node *DChat
	)
	if len(duo.hooks) == 0 {
		node, err = duo.sqlSave(ctx)
	} else {
		var mut Mutator = MutateFunc(func(ctx context.Context, m Mutation) (Value, error) {
			mutation, ok := m.(*DChatMutation)
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
func (duo *DChatUpdateOne) SaveX(ctx context.Context) *DChat {
	node, err := duo.Save(ctx)
	if err != nil {
		panic(err)
	}
	return node
}

// Exec executes the query on the entity.
func (duo *DChatUpdateOne) Exec(ctx context.Context) error {
	_, err := duo.Save(ctx)
	return err
}

// ExecX is like Exec, but panics if an error occurs.
func (duo *DChatUpdateOne) ExecX(ctx context.Context) {
	if err := duo.Exec(ctx); err != nil {
		panic(err)
	}
}

func (duo *DChatUpdateOne) sqlSave(ctx context.Context) (_node *DChat, err error) {
	_spec := &sqlgraph.UpdateSpec{
		Node: &sqlgraph.NodeSpec{
			Table:   dchat.Table,
			Columns: dchat.Columns,
			ID: &sqlgraph.FieldSpec{
				Type:   field.TypeString,
				Column: dchat.FieldID,
			},
		},
	}
	id, ok := duo.mutation.ID()
	if !ok {
		return nil, &ValidationError{Name: "ID", err: fmt.Errorf("missing DChat.ID for update")}
	}
	_spec.Node.ID.Value = id
	if fields := duo.fields; len(fields) > 0 {
		_spec.Node.Columns = make([]string, 0, len(fields))
		_spec.Node.Columns = append(_spec.Node.Columns, dchat.FieldID)
		for _, f := range fields {
			if !dchat.ValidColumn(f) {
				return nil, &ValidationError{Name: f, err: fmt.Errorf("ent: invalid field %q for query", f)}
			}
			if f != dchat.FieldID {
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
	if value, ok := duo.mutation.SessionID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldSessionID,
		})
	}
	if value, ok := duo.mutation.UserID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldUserID,
		})
	}
	if value, ok := duo.mutation.RoomID(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldRoomID,
		})
	}
	if value, ok := duo.mutation.Text(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: dchat.FieldText,
		})
	}
	if value, ok := duo.mutation.CreateTime(); ok {
		_spec.Fields.Set = append(_spec.Fields.Set, &sqlgraph.FieldSpec{
			Type:   field.TypeTime,
			Value:  value,
			Column: dchat.FieldCreateTime,
		})
	}
	_node = &DChat{config: duo.config}
	_spec.Assign = _node.assignValues
	_spec.ScanValues = _node.scanValues
	if err = sqlgraph.UpdateNode(ctx, duo.driver, _spec); err != nil {
		if _, ok := err.(*sqlgraph.NotFoundError); ok {
			err = &NotFoundError{dchat.Label}
		} else if sqlgraph.IsConstraintError(err) {
			err = &ConstraintError{err.Error(), err}
		}
		return nil, err
	}
	return _node, nil
}
