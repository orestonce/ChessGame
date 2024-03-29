// Code generated by entc, DO NOT EDIT.

package ent

import (
	"context"
	"fmt"
	"log"

	"github.com/orestonce/ChessGame/ent/migrate"

	"github.com/orestonce/ChessGame/ent/dchat"
	"github.com/orestonce/ChessGame/ent/dchessdbcache"
	"github.com/orestonce/ChessGame/ent/droom"
	"github.com/orestonce/ChessGame/ent/dsession"
	"github.com/orestonce/ChessGame/ent/duser"

	"entgo.io/ent/dialect"
	"entgo.io/ent/dialect/sql"
)

// Client is the client that holds all ent builders.
type Client struct {
	config
	// Schema is the client for creating, migrating and dropping schema.
	Schema *migrate.Schema
	// DChat is the client for interacting with the DChat builders.
	DChat *DChatClient
	// DChessdbCache is the client for interacting with the DChessdbCache builders.
	DChessdbCache *DChessdbCacheClient
	// DRoom is the client for interacting with the DRoom builders.
	DRoom *DRoomClient
	// DSession is the client for interacting with the DSession builders.
	DSession *DSessionClient
	// DUser is the client for interacting with the DUser builders.
	DUser *DUserClient
}

// NewClient creates a new client configured with the given options.
func NewClient(opts ...Option) *Client {
	cfg := config{log: log.Println, hooks: &hooks{}}
	cfg.options(opts...)
	client := &Client{config: cfg}
	client.init()
	return client
}

func (c *Client) init() {
	c.Schema = migrate.NewSchema(c.driver)
	c.DChat = NewDChatClient(c.config)
	c.DChessdbCache = NewDChessdbCacheClient(c.config)
	c.DRoom = NewDRoomClient(c.config)
	c.DSession = NewDSessionClient(c.config)
	c.DUser = NewDUserClient(c.config)
}

// Open opens a database/sql.DB specified by the driver name and
// the data source name, and returns a new client attached to it.
// Optional parameters can be added for configuring the client.
func Open(driverName, dataSourceName string, options ...Option) (*Client, error) {
	switch driverName {
	case dialect.MySQL, dialect.Postgres, dialect.SQLite:
		drv, err := sql.Open(driverName, dataSourceName)
		if err != nil {
			return nil, err
		}
		return NewClient(append(options, Driver(drv))...), nil
	default:
		return nil, fmt.Errorf("unsupported driver: %q", driverName)
	}
}

// Tx returns a new transactional client. The provided context
// is used until the transaction is committed or rolled back.
func (c *Client) Tx(ctx context.Context) (*Tx, error) {
	if _, ok := c.driver.(*txDriver); ok {
		return nil, fmt.Errorf("ent: cannot start a transaction within a transaction")
	}
	tx, err := newTx(ctx, c.driver)
	if err != nil {
		return nil, fmt.Errorf("ent: starting a transaction: %w", err)
	}
	cfg := c.config
	cfg.driver = tx
	return &Tx{
		ctx:           ctx,
		config:        cfg,
		DChat:         NewDChatClient(cfg),
		DChessdbCache: NewDChessdbCacheClient(cfg),
		DRoom:         NewDRoomClient(cfg),
		DSession:      NewDSessionClient(cfg),
		DUser:         NewDUserClient(cfg),
	}, nil
}

// BeginTx returns a transactional client with specified options.
func (c *Client) BeginTx(ctx context.Context, opts *sql.TxOptions) (*Tx, error) {
	if _, ok := c.driver.(*txDriver); ok {
		return nil, fmt.Errorf("ent: cannot start a transaction within a transaction")
	}
	tx, err := c.driver.(interface {
		BeginTx(context.Context, *sql.TxOptions) (dialect.Tx, error)
	}).BeginTx(ctx, opts)
	if err != nil {
		return nil, fmt.Errorf("ent: starting a transaction: %w", err)
	}
	cfg := c.config
	cfg.driver = &txDriver{tx: tx, drv: c.driver}
	return &Tx{
		config:        cfg,
		DChat:         NewDChatClient(cfg),
		DChessdbCache: NewDChessdbCacheClient(cfg),
		DRoom:         NewDRoomClient(cfg),
		DSession:      NewDSessionClient(cfg),
		DUser:         NewDUserClient(cfg),
	}, nil
}

// Debug returns a new debug-client. It's used to get verbose logging on specific operations.
//
//	client.Debug().
//		DChat.
//		Query().
//		Count(ctx)
//
func (c *Client) Debug() *Client {
	if c.debug {
		return c
	}
	cfg := c.config
	cfg.driver = dialect.Debug(c.driver, c.log)
	client := &Client{config: cfg}
	client.init()
	return client
}

// Close closes the database connection and prevents new queries from starting.
func (c *Client) Close() error {
	return c.driver.Close()
}

// Use adds the mutation hooks to all the entity clients.
// In order to add hooks to a specific client, call: `client.Node.Use(...)`.
func (c *Client) Use(hooks ...Hook) {
	c.DChat.Use(hooks...)
	c.DChessdbCache.Use(hooks...)
	c.DRoom.Use(hooks...)
	c.DSession.Use(hooks...)
	c.DUser.Use(hooks...)
}

// DChatClient is a client for the DChat schema.
type DChatClient struct {
	config
}

// NewDChatClient returns a client for the DChat from the given config.
func NewDChatClient(c config) *DChatClient {
	return &DChatClient{config: c}
}

// Use adds a list of mutation hooks to the hooks stack.
// A call to `Use(f, g, h)` equals to `dchat.Hooks(f(g(h())))`.
func (c *DChatClient) Use(hooks ...Hook) {
	c.hooks.DChat = append(c.hooks.DChat, hooks...)
}

// Create returns a create builder for DChat.
func (c *DChatClient) Create() *DChatCreate {
	mutation := newDChatMutation(c.config, OpCreate)
	return &DChatCreate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// CreateBulk returns a builder for creating a bulk of DChat entities.
func (c *DChatClient) CreateBulk(builders ...*DChatCreate) *DChatCreateBulk {
	return &DChatCreateBulk{config: c.config, builders: builders}
}

// Update returns an update builder for DChat.
func (c *DChatClient) Update() *DChatUpdate {
	mutation := newDChatMutation(c.config, OpUpdate)
	return &DChatUpdate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOne returns an update builder for the given entity.
func (c *DChatClient) UpdateOne(d *DChat) *DChatUpdateOne {
	mutation := newDChatMutation(c.config, OpUpdateOne, withDChat(d))
	return &DChatUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOneID returns an update builder for the given id.
func (c *DChatClient) UpdateOneID(id string) *DChatUpdateOne {
	mutation := newDChatMutation(c.config, OpUpdateOne, withDChatID(id))
	return &DChatUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// Delete returns a delete builder for DChat.
func (c *DChatClient) Delete() *DChatDelete {
	mutation := newDChatMutation(c.config, OpDelete)
	return &DChatDelete{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// DeleteOne returns a delete builder for the given entity.
func (c *DChatClient) DeleteOne(d *DChat) *DChatDeleteOne {
	return c.DeleteOneID(d.ID)
}

// DeleteOneID returns a delete builder for the given id.
func (c *DChatClient) DeleteOneID(id string) *DChatDeleteOne {
	builder := c.Delete().Where(dchat.ID(id))
	builder.mutation.id = &id
	builder.mutation.op = OpDeleteOne
	return &DChatDeleteOne{builder}
}

// Query returns a query builder for DChat.
func (c *DChatClient) Query() *DChatQuery {
	return &DChatQuery{
		config: c.config,
	}
}

// Get returns a DChat entity by its id.
func (c *DChatClient) Get(ctx context.Context, id string) (*DChat, error) {
	return c.Query().Where(dchat.ID(id)).Only(ctx)
}

// GetX is like Get, but panics if an error occurs.
func (c *DChatClient) GetX(ctx context.Context, id string) *DChat {
	obj, err := c.Get(ctx, id)
	if err != nil {
		panic(err)
	}
	return obj
}

// Hooks returns the client hooks.
func (c *DChatClient) Hooks() []Hook {
	return c.hooks.DChat
}

// DChessdbCacheClient is a client for the DChessdbCache schema.
type DChessdbCacheClient struct {
	config
}

// NewDChessdbCacheClient returns a client for the DChessdbCache from the given config.
func NewDChessdbCacheClient(c config) *DChessdbCacheClient {
	return &DChessdbCacheClient{config: c}
}

// Use adds a list of mutation hooks to the hooks stack.
// A call to `Use(f, g, h)` equals to `dchessdbcache.Hooks(f(g(h())))`.
func (c *DChessdbCacheClient) Use(hooks ...Hook) {
	c.hooks.DChessdbCache = append(c.hooks.DChessdbCache, hooks...)
}

// Create returns a create builder for DChessdbCache.
func (c *DChessdbCacheClient) Create() *DChessdbCacheCreate {
	mutation := newDChessdbCacheMutation(c.config, OpCreate)
	return &DChessdbCacheCreate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// CreateBulk returns a builder for creating a bulk of DChessdbCache entities.
func (c *DChessdbCacheClient) CreateBulk(builders ...*DChessdbCacheCreate) *DChessdbCacheCreateBulk {
	return &DChessdbCacheCreateBulk{config: c.config, builders: builders}
}

// Update returns an update builder for DChessdbCache.
func (c *DChessdbCacheClient) Update() *DChessdbCacheUpdate {
	mutation := newDChessdbCacheMutation(c.config, OpUpdate)
	return &DChessdbCacheUpdate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOne returns an update builder for the given entity.
func (c *DChessdbCacheClient) UpdateOne(dc *DChessdbCache) *DChessdbCacheUpdateOne {
	mutation := newDChessdbCacheMutation(c.config, OpUpdateOne, withDChessdbCache(dc))
	return &DChessdbCacheUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOneID returns an update builder for the given id.
func (c *DChessdbCacheClient) UpdateOneID(id string) *DChessdbCacheUpdateOne {
	mutation := newDChessdbCacheMutation(c.config, OpUpdateOne, withDChessdbCacheID(id))
	return &DChessdbCacheUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// Delete returns a delete builder for DChessdbCache.
func (c *DChessdbCacheClient) Delete() *DChessdbCacheDelete {
	mutation := newDChessdbCacheMutation(c.config, OpDelete)
	return &DChessdbCacheDelete{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// DeleteOne returns a delete builder for the given entity.
func (c *DChessdbCacheClient) DeleteOne(dc *DChessdbCache) *DChessdbCacheDeleteOne {
	return c.DeleteOneID(dc.ID)
}

// DeleteOneID returns a delete builder for the given id.
func (c *DChessdbCacheClient) DeleteOneID(id string) *DChessdbCacheDeleteOne {
	builder := c.Delete().Where(dchessdbcache.ID(id))
	builder.mutation.id = &id
	builder.mutation.op = OpDeleteOne
	return &DChessdbCacheDeleteOne{builder}
}

// Query returns a query builder for DChessdbCache.
func (c *DChessdbCacheClient) Query() *DChessdbCacheQuery {
	return &DChessdbCacheQuery{
		config: c.config,
	}
}

// Get returns a DChessdbCache entity by its id.
func (c *DChessdbCacheClient) Get(ctx context.Context, id string) (*DChessdbCache, error) {
	return c.Query().Where(dchessdbcache.ID(id)).Only(ctx)
}

// GetX is like Get, but panics if an error occurs.
func (c *DChessdbCacheClient) GetX(ctx context.Context, id string) *DChessdbCache {
	obj, err := c.Get(ctx, id)
	if err != nil {
		panic(err)
	}
	return obj
}

// Hooks returns the client hooks.
func (c *DChessdbCacheClient) Hooks() []Hook {
	return c.hooks.DChessdbCache
}

// DRoomClient is a client for the DRoom schema.
type DRoomClient struct {
	config
}

// NewDRoomClient returns a client for the DRoom from the given config.
func NewDRoomClient(c config) *DRoomClient {
	return &DRoomClient{config: c}
}

// Use adds a list of mutation hooks to the hooks stack.
// A call to `Use(f, g, h)` equals to `droom.Hooks(f(g(h())))`.
func (c *DRoomClient) Use(hooks ...Hook) {
	c.hooks.DRoom = append(c.hooks.DRoom, hooks...)
}

// Create returns a create builder for DRoom.
func (c *DRoomClient) Create() *DRoomCreate {
	mutation := newDRoomMutation(c.config, OpCreate)
	return &DRoomCreate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// CreateBulk returns a builder for creating a bulk of DRoom entities.
func (c *DRoomClient) CreateBulk(builders ...*DRoomCreate) *DRoomCreateBulk {
	return &DRoomCreateBulk{config: c.config, builders: builders}
}

// Update returns an update builder for DRoom.
func (c *DRoomClient) Update() *DRoomUpdate {
	mutation := newDRoomMutation(c.config, OpUpdate)
	return &DRoomUpdate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOne returns an update builder for the given entity.
func (c *DRoomClient) UpdateOne(d *DRoom) *DRoomUpdateOne {
	mutation := newDRoomMutation(c.config, OpUpdateOne, withDRoom(d))
	return &DRoomUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOneID returns an update builder for the given id.
func (c *DRoomClient) UpdateOneID(id string) *DRoomUpdateOne {
	mutation := newDRoomMutation(c.config, OpUpdateOne, withDRoomID(id))
	return &DRoomUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// Delete returns a delete builder for DRoom.
func (c *DRoomClient) Delete() *DRoomDelete {
	mutation := newDRoomMutation(c.config, OpDelete)
	return &DRoomDelete{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// DeleteOne returns a delete builder for the given entity.
func (c *DRoomClient) DeleteOne(d *DRoom) *DRoomDeleteOne {
	return c.DeleteOneID(d.ID)
}

// DeleteOneID returns a delete builder for the given id.
func (c *DRoomClient) DeleteOneID(id string) *DRoomDeleteOne {
	builder := c.Delete().Where(droom.ID(id))
	builder.mutation.id = &id
	builder.mutation.op = OpDeleteOne
	return &DRoomDeleteOne{builder}
}

// Query returns a query builder for DRoom.
func (c *DRoomClient) Query() *DRoomQuery {
	return &DRoomQuery{
		config: c.config,
	}
}

// Get returns a DRoom entity by its id.
func (c *DRoomClient) Get(ctx context.Context, id string) (*DRoom, error) {
	return c.Query().Where(droom.ID(id)).Only(ctx)
}

// GetX is like Get, but panics if an error occurs.
func (c *DRoomClient) GetX(ctx context.Context, id string) *DRoom {
	obj, err := c.Get(ctx, id)
	if err != nil {
		panic(err)
	}
	return obj
}

// Hooks returns the client hooks.
func (c *DRoomClient) Hooks() []Hook {
	return c.hooks.DRoom
}

// DSessionClient is a client for the DSession schema.
type DSessionClient struct {
	config
}

// NewDSessionClient returns a client for the DSession from the given config.
func NewDSessionClient(c config) *DSessionClient {
	return &DSessionClient{config: c}
}

// Use adds a list of mutation hooks to the hooks stack.
// A call to `Use(f, g, h)` equals to `dsession.Hooks(f(g(h())))`.
func (c *DSessionClient) Use(hooks ...Hook) {
	c.hooks.DSession = append(c.hooks.DSession, hooks...)
}

// Create returns a create builder for DSession.
func (c *DSessionClient) Create() *DSessionCreate {
	mutation := newDSessionMutation(c.config, OpCreate)
	return &DSessionCreate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// CreateBulk returns a builder for creating a bulk of DSession entities.
func (c *DSessionClient) CreateBulk(builders ...*DSessionCreate) *DSessionCreateBulk {
	return &DSessionCreateBulk{config: c.config, builders: builders}
}

// Update returns an update builder for DSession.
func (c *DSessionClient) Update() *DSessionUpdate {
	mutation := newDSessionMutation(c.config, OpUpdate)
	return &DSessionUpdate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOne returns an update builder for the given entity.
func (c *DSessionClient) UpdateOne(d *DSession) *DSessionUpdateOne {
	mutation := newDSessionMutation(c.config, OpUpdateOne, withDSession(d))
	return &DSessionUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOneID returns an update builder for the given id.
func (c *DSessionClient) UpdateOneID(id string) *DSessionUpdateOne {
	mutation := newDSessionMutation(c.config, OpUpdateOne, withDSessionID(id))
	return &DSessionUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// Delete returns a delete builder for DSession.
func (c *DSessionClient) Delete() *DSessionDelete {
	mutation := newDSessionMutation(c.config, OpDelete)
	return &DSessionDelete{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// DeleteOne returns a delete builder for the given entity.
func (c *DSessionClient) DeleteOne(d *DSession) *DSessionDeleteOne {
	return c.DeleteOneID(d.ID)
}

// DeleteOneID returns a delete builder for the given id.
func (c *DSessionClient) DeleteOneID(id string) *DSessionDeleteOne {
	builder := c.Delete().Where(dsession.ID(id))
	builder.mutation.id = &id
	builder.mutation.op = OpDeleteOne
	return &DSessionDeleteOne{builder}
}

// Query returns a query builder for DSession.
func (c *DSessionClient) Query() *DSessionQuery {
	return &DSessionQuery{
		config: c.config,
	}
}

// Get returns a DSession entity by its id.
func (c *DSessionClient) Get(ctx context.Context, id string) (*DSession, error) {
	return c.Query().Where(dsession.ID(id)).Only(ctx)
}

// GetX is like Get, but panics if an error occurs.
func (c *DSessionClient) GetX(ctx context.Context, id string) *DSession {
	obj, err := c.Get(ctx, id)
	if err != nil {
		panic(err)
	}
	return obj
}

// Hooks returns the client hooks.
func (c *DSessionClient) Hooks() []Hook {
	return c.hooks.DSession
}

// DUserClient is a client for the DUser schema.
type DUserClient struct {
	config
}

// NewDUserClient returns a client for the DUser from the given config.
func NewDUserClient(c config) *DUserClient {
	return &DUserClient{config: c}
}

// Use adds a list of mutation hooks to the hooks stack.
// A call to `Use(f, g, h)` equals to `duser.Hooks(f(g(h())))`.
func (c *DUserClient) Use(hooks ...Hook) {
	c.hooks.DUser = append(c.hooks.DUser, hooks...)
}

// Create returns a create builder for DUser.
func (c *DUserClient) Create() *DUserCreate {
	mutation := newDUserMutation(c.config, OpCreate)
	return &DUserCreate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// CreateBulk returns a builder for creating a bulk of DUser entities.
func (c *DUserClient) CreateBulk(builders ...*DUserCreate) *DUserCreateBulk {
	return &DUserCreateBulk{config: c.config, builders: builders}
}

// Update returns an update builder for DUser.
func (c *DUserClient) Update() *DUserUpdate {
	mutation := newDUserMutation(c.config, OpUpdate)
	return &DUserUpdate{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOne returns an update builder for the given entity.
func (c *DUserClient) UpdateOne(d *DUser) *DUserUpdateOne {
	mutation := newDUserMutation(c.config, OpUpdateOne, withDUser(d))
	return &DUserUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// UpdateOneID returns an update builder for the given id.
func (c *DUserClient) UpdateOneID(id string) *DUserUpdateOne {
	mutation := newDUserMutation(c.config, OpUpdateOne, withDUserID(id))
	return &DUserUpdateOne{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// Delete returns a delete builder for DUser.
func (c *DUserClient) Delete() *DUserDelete {
	mutation := newDUserMutation(c.config, OpDelete)
	return &DUserDelete{config: c.config, hooks: c.Hooks(), mutation: mutation}
}

// DeleteOne returns a delete builder for the given entity.
func (c *DUserClient) DeleteOne(d *DUser) *DUserDeleteOne {
	return c.DeleteOneID(d.ID)
}

// DeleteOneID returns a delete builder for the given id.
func (c *DUserClient) DeleteOneID(id string) *DUserDeleteOne {
	builder := c.Delete().Where(duser.ID(id))
	builder.mutation.id = &id
	builder.mutation.op = OpDeleteOne
	return &DUserDeleteOne{builder}
}

// Query returns a query builder for DUser.
func (c *DUserClient) Query() *DUserQuery {
	return &DUserQuery{
		config: c.config,
	}
}

// Get returns a DUser entity by its id.
func (c *DUserClient) Get(ctx context.Context, id string) (*DUser, error) {
	return c.Query().Where(duser.ID(id)).Only(ctx)
}

// GetX is like Get, but panics if an error occurs.
func (c *DUserClient) GetX(ctx context.Context, id string) *DUser {
	obj, err := c.Get(ctx, id)
	if err != nil {
		panic(err)
	}
	return obj
}

// Hooks returns the client hooks.
func (c *DUserClient) Hooks() []Hook {
	return c.hooks.DUser
}
