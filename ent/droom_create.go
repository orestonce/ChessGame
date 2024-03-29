// Code generated by entc, DO NOT EDIT.

package ent

import (
	"context"
	"errors"
	"fmt"
	"time"

	"entgo.io/ent/dialect/sql/sqlgraph"
	"entgo.io/ent/schema/field"
	"github.com/orestonce/ChessGame/ent/droom"
)

// DRoomCreate is the builder for creating a DRoom entity.
type DRoomCreate struct {
	config
	mutation *DRoomMutation
	hooks    []Hook
}

// SetIsGameRunning sets the "is_game_running" field.
func (dc *DRoomCreate) SetIsGameRunning(b bool) *DRoomCreate {
	dc.mutation.SetIsGameRunning(b)
	return dc
}

// SetNillableIsGameRunning sets the "is_game_running" field if the given value is not nil.
func (dc *DRoomCreate) SetNillableIsGameRunning(b *bool) *DRoomCreate {
	if b != nil {
		dc.SetIsGameRunning(*b)
	}
	return dc
}

// SetPanel sets the "panel" field.
func (dc *DRoomCreate) SetPanel(s string) *DRoomCreate {
	dc.mutation.SetPanel(s)
	return dc
}

// SetNillablePanel sets the "panel" field if the given value is not nil.
func (dc *DRoomCreate) SetNillablePanel(s *string) *DRoomCreate {
	if s != nil {
		dc.SetPanel(*s)
	}
	return dc
}

// SetWUserID sets the "w_user_id" field.
func (dc *DRoomCreate) SetWUserID(s string) *DRoomCreate {
	dc.mutation.SetWUserID(s)
	return dc
}

// SetNillableWUserID sets the "w_user_id" field if the given value is not nil.
func (dc *DRoomCreate) SetNillableWUserID(s *string) *DRoomCreate {
	if s != nil {
		dc.SetWUserID(*s)
	}
	return dc
}

// SetBUserID sets the "b_user_id" field.
func (dc *DRoomCreate) SetBUserID(s string) *DRoomCreate {
	dc.mutation.SetBUserID(s)
	return dc
}

// SetNillableBUserID sets the "b_user_id" field if the given value is not nil.
func (dc *DRoomCreate) SetNillableBUserID(s *string) *DRoomCreate {
	if s != nil {
		dc.SetBUserID(*s)
	}
	return dc
}

// SetCreateTime sets the "create_time" field.
func (dc *DRoomCreate) SetCreateTime(t time.Time) *DRoomCreate {
	dc.mutation.SetCreateTime(t)
	return dc
}

// SetID sets the "id" field.
func (dc *DRoomCreate) SetID(s string) *DRoomCreate {
	dc.mutation.SetID(s)
	return dc
}

// Mutation returns the DRoomMutation object of the builder.
func (dc *DRoomCreate) Mutation() *DRoomMutation {
	return dc.mutation
}

// Save creates the DRoom in the database.
func (dc *DRoomCreate) Save(ctx context.Context) (*DRoom, error) {
	var (
		err  error
		node *DRoom
	)
	dc.defaults()
	if len(dc.hooks) == 0 {
		if err = dc.check(); err != nil {
			return nil, err
		}
		node, err = dc.sqlSave(ctx)
	} else {
		var mut Mutator = MutateFunc(func(ctx context.Context, m Mutation) (Value, error) {
			mutation, ok := m.(*DRoomMutation)
			if !ok {
				return nil, fmt.Errorf("unexpected mutation type %T", m)
			}
			if err = dc.check(); err != nil {
				return nil, err
			}
			dc.mutation = mutation
			if node, err = dc.sqlSave(ctx); err != nil {
				return nil, err
			}
			mutation.id = &node.ID
			mutation.done = true
			return node, err
		})
		for i := len(dc.hooks) - 1; i >= 0; i-- {
			if dc.hooks[i] == nil {
				return nil, fmt.Errorf("ent: uninitialized hook (forgotten import ent/runtime?)")
			}
			mut = dc.hooks[i](mut)
		}
		if _, err := mut.Mutate(ctx, dc.mutation); err != nil {
			return nil, err
		}
	}
	return node, err
}

// SaveX calls Save and panics if Save returns an error.
func (dc *DRoomCreate) SaveX(ctx context.Context) *DRoom {
	v, err := dc.Save(ctx)
	if err != nil {
		panic(err)
	}
	return v
}

// Exec executes the query.
func (dc *DRoomCreate) Exec(ctx context.Context) error {
	_, err := dc.Save(ctx)
	return err
}

// ExecX is like Exec, but panics if an error occurs.
func (dc *DRoomCreate) ExecX(ctx context.Context) {
	if err := dc.Exec(ctx); err != nil {
		panic(err)
	}
}

// defaults sets the default values of the builder before save.
func (dc *DRoomCreate) defaults() {
	if _, ok := dc.mutation.IsGameRunning(); !ok {
		v := droom.DefaultIsGameRunning
		dc.mutation.SetIsGameRunning(v)
	}
	if _, ok := dc.mutation.Panel(); !ok {
		v := droom.DefaultPanel
		dc.mutation.SetPanel(v)
	}
	if _, ok := dc.mutation.WUserID(); !ok {
		v := droom.DefaultWUserID
		dc.mutation.SetWUserID(v)
	}
	if _, ok := dc.mutation.BUserID(); !ok {
		v := droom.DefaultBUserID
		dc.mutation.SetBUserID(v)
	}
}

// check runs all checks and user-defined validators on the builder.
func (dc *DRoomCreate) check() error {
	if _, ok := dc.mutation.CreateTime(); !ok {
		return &ValidationError{Name: "create_time", err: errors.New(`ent: missing required field "create_time"`)}
	}
	return nil
}

func (dc *DRoomCreate) sqlSave(ctx context.Context) (*DRoom, error) {
	_node, _spec := dc.createSpec()
	if err := sqlgraph.CreateNode(ctx, dc.driver, _spec); err != nil {
		if sqlgraph.IsConstraintError(err) {
			err = &ConstraintError{err.Error(), err}
		}
		return nil, err
	}
	return _node, nil
}

func (dc *DRoomCreate) createSpec() (*DRoom, *sqlgraph.CreateSpec) {
	var (
		_node = &DRoom{config: dc.config}
		_spec = &sqlgraph.CreateSpec{
			Table: droom.Table,
			ID: &sqlgraph.FieldSpec{
				Type:   field.TypeString,
				Column: droom.FieldID,
			},
		}
	)
	if id, ok := dc.mutation.ID(); ok {
		_node.ID = id
		_spec.ID.Value = id
	}
	if value, ok := dc.mutation.IsGameRunning(); ok {
		_spec.Fields = append(_spec.Fields, &sqlgraph.FieldSpec{
			Type:   field.TypeBool,
			Value:  value,
			Column: droom.FieldIsGameRunning,
		})
		_node.IsGameRunning = value
	}
	if value, ok := dc.mutation.Panel(); ok {
		_spec.Fields = append(_spec.Fields, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: droom.FieldPanel,
		})
		_node.Panel = value
	}
	if value, ok := dc.mutation.WUserID(); ok {
		_spec.Fields = append(_spec.Fields, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: droom.FieldWUserID,
		})
		_node.WUserID = value
	}
	if value, ok := dc.mutation.BUserID(); ok {
		_spec.Fields = append(_spec.Fields, &sqlgraph.FieldSpec{
			Type:   field.TypeString,
			Value:  value,
			Column: droom.FieldBUserID,
		})
		_node.BUserID = value
	}
	if value, ok := dc.mutation.CreateTime(); ok {
		_spec.Fields = append(_spec.Fields, &sqlgraph.FieldSpec{
			Type:   field.TypeTime,
			Value:  value,
			Column: droom.FieldCreateTime,
		})
		_node.CreateTime = value
	}
	return _node, _spec
}

// DRoomCreateBulk is the builder for creating many DRoom entities in bulk.
type DRoomCreateBulk struct {
	config
	builders []*DRoomCreate
}

// Save creates the DRoom entities in the database.
func (dcb *DRoomCreateBulk) Save(ctx context.Context) ([]*DRoom, error) {
	specs := make([]*sqlgraph.CreateSpec, len(dcb.builders))
	nodes := make([]*DRoom, len(dcb.builders))
	mutators := make([]Mutator, len(dcb.builders))
	for i := range dcb.builders {
		func(i int, root context.Context) {
			builder := dcb.builders[i]
			builder.defaults()
			var mut Mutator = MutateFunc(func(ctx context.Context, m Mutation) (Value, error) {
				mutation, ok := m.(*DRoomMutation)
				if !ok {
					return nil, fmt.Errorf("unexpected mutation type %T", m)
				}
				if err := builder.check(); err != nil {
					return nil, err
				}
				builder.mutation = mutation
				nodes[i], specs[i] = builder.createSpec()
				var err error
				if i < len(mutators)-1 {
					_, err = mutators[i+1].Mutate(root, dcb.builders[i+1].mutation)
				} else {
					spec := &sqlgraph.BatchCreateSpec{Nodes: specs}
					// Invoke the actual operation on the latest mutation in the chain.
					if err = sqlgraph.BatchCreate(ctx, dcb.driver, spec); err != nil {
						if sqlgraph.IsConstraintError(err) {
							err = &ConstraintError{err.Error(), err}
						}
					}
				}
				if err != nil {
					return nil, err
				}
				mutation.id = &nodes[i].ID
				mutation.done = true
				return nodes[i], nil
			})
			for i := len(builder.hooks) - 1; i >= 0; i-- {
				mut = builder.hooks[i](mut)
			}
			mutators[i] = mut
		}(i, ctx)
	}
	if len(mutators) > 0 {
		if _, err := mutators[0].Mutate(ctx, dcb.builders[0].mutation); err != nil {
			return nil, err
		}
	}
	return nodes, nil
}

// SaveX is like Save, but panics if an error occurs.
func (dcb *DRoomCreateBulk) SaveX(ctx context.Context) []*DRoom {
	v, err := dcb.Save(ctx)
	if err != nil {
		panic(err)
	}
	return v
}

// Exec executes the query.
func (dcb *DRoomCreateBulk) Exec(ctx context.Context) error {
	_, err := dcb.Save(ctx)
	return err
}

// ExecX is like Exec, but panics if an error occurs.
func (dcb *DRoomCreateBulk) ExecX(ctx context.Context) {
	if err := dcb.Exec(ctx); err != nil {
		panic(err)
	}
}
