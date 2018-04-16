#ifndef ZMQ_HPP_
#define ZMQ_HPP_

#include <zmq.h>

#include <algorithm>
#include <cstring>
#include <exception>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

namespace polo {
namespace communicator {
namespace zmq {
/* Version information */
void version(int &major, int &minor, int &patch) {
  zmq_version(&major, &minor, &patch);
}

std::tuple<int, int, int> version() {
  int major, minor, patch;
  version(major, minor, patch);
  return std::make_tuple(major, minor, patch);
}

/* Error handling through exceptions */
struct error : public std::exception {
  error() noexcept : errno_{zmq_errno()} {}
  const char *what() const noexcept override { return zmq_strerror(errno_); }
  operator int() const noexcept { return errno_; }

private:
  int errno_;
};

/* Forward declarations and definitions */
struct context;
struct socket;
struct message;
struct poller;

enum struct context_opt : int;
enum struct socket_type : int;
enum struct socket_opt : int;
enum struct monitor_event : int;
enum struct poll_event : short;

struct context {
  context();

  void set(context_opt, int) const;
  int get(context_opt) const;

  explicit operator void *() const noexcept;
  explicit operator bool() const noexcept;

private:
  std::shared_ptr<void> ptr_;
};

struct socket {
  socket(const context &, socket_type);

  template <class T> void set(socket_opt, const T &) const;
  template <class T> void get(socket_opt, T &) const;

  void bind(const char *) const;
  void connect(const char *) const;
  void unbind(const char *) const;
  void disconnect(const char *) const;

  int send(message, bool = true) const;
  template <class T> int send(std::size_t, const T *, int) const;

  message receive(bool = true) const;
  template <class T> int receive(std::size_t, T *, int) const;

  socket monitor(const context &, const char *, monitor_event) const;
  socket monitor(const context &, const char *, int) const;

  explicit operator void *() const noexcept;

private:
  std::unique_ptr<void, void (*)(void *)> ptr_;
};

struct message {
private:
  struct part {
    /* Constructors */
    part() noexcept;
    explicit part(std::size_t);
    template <class T>
    part(const T &,
         typename std::enable_if<std::is_arithmetic<T>::value>::type * =
             nullptr);
    template <class T> part(std::size_t, const T *);
    template <class T> part(std::size_t, T *, zmq_free_fn *, void * = nullptr);
    template <class Iter> part(Iter, Iter);

    /* Copying and moving */
    part(const part &) noexcept;
    part &operator=(const part &) noexcept;
    part(part &&) noexcept;
    part &operator=(part &&) noexcept;

    /* Destruction */
    ~part();

    /* Convenience */
    std::size_t size() const noexcept;
    explicit operator void *() noexcept;
    void *data() noexcept;
    bool more() const noexcept;

    int send(const socket &, int);
    int receive(const socket &, int);
    template <class OutputIt> OutputIt read(OutputIt, OutputIt) noexcept;
    void clear() noexcept;

  private:
    zmq_msg_t msg_;
  };

  std::vector<part> parts_;

public:
  message() noexcept(noexcept(std::vector<part>()));
  message(std::vector<part>) noexcept;

  std::size_t addpart();
  std::size_t addpart(std::size_t);
  template <class T> std::size_t addpart(const T &);
  template <class T> std::size_t addpart(std::size_t, const T *);
  template <class T>
  std::size_t addpart(std::size_t, T *, zmq_free_fn *, void * = nullptr);
  template <class Iter> std::size_t addpart(Iter, Iter);

  void *data(std::size_t) noexcept;

  std::size_t numparts() const noexcept;
  std::size_t size() const noexcept;
  std::size_t size(std::size_t) const noexcept;

  part &operator[](std::size_t);
  const part &operator[](std::size_t) const;

  void pop_back();

  int send(const socket &, bool = true);
  int receive(const socket &, bool = true);
  template <class OutputIt>
  OutputIt read(std::size_t, OutputIt, OutputIt) noexcept;
  void clear() noexcept;
};

struct poller {
private:
  struct item {
    explicit item(const zmq_pollitem_t) noexcept;

    bool belongsto(const socket &) const noexcept;
    bool isready() const noexcept;

  private:
    const zmq_pollitem_t item_;
  };

  std::vector<zmq_pollitem_t> items_;

public:
  poller() noexcept(noexcept(std::vector<zmq_pollitem_t>()));

  void additem(const socket &, poll_event);
  void additem(const socket &, short);

  std::size_t size() const noexcept;
  item operator[](std::size_t) const;

  int poll(long);

  void clear() noexcept;
};

/* Proxy functionality */
void proxy(const socket *frontend, const socket *backend,
           const socket *capture = nullptr) {
  void *f = static_cast<void *>(*frontend);
  void *b = static_cast<void *>(*backend);
  void *c = (capture == nullptr) ? nullptr : static_cast<void *>(*capture);
  zmq_proxy(f, b, c);
}
void proxy(const socket *frontend, const socket *backend, const socket *capture,
           const socket *control) {
  void *f = static_cast<void *>(*frontend);
  void *b = static_cast<void *>(*backend);
  void *cap = (capture == nullptr) ? nullptr : static_cast<void *>(*capture);
  void *con = static_cast<void *>(*control);
  zmq_proxy_steerable(f, b, cap, con);
}

/* Implementations */
enum struct context_opt : int {
#ifdef ZMQ_IO_THREADS
  io_threads = ZMQ_IO_THREADS,
#endif
#ifdef ZMQ_MAX_SOCKETS
  max_sockets = ZMQ_MAX_SOCKETS,
#endif
#ifdef ZMQ_SOCKET_LIMIT
  socket_limit = ZMQ_SOCKET_LIMIT,
#endif
#ifdef ZMQ_THREAD_PRIORITY
  thread_priority = ZMQ_THREAD_PRIORITY,
#endif
#ifdef ZMQ_THREAD_SCHED_POLICY
  thread_sched_policy = ZMQ_THREAD_SCHED_POLICY,
#endif
#ifdef ZMQ_MAX_MSGSZ
  max_msgsz = ZMQ_MAX_MSGSZ,
#endif
#ifdef ZMQ_MSG_T_SIZE
  msg_t_size = ZMQ_MSG_T_SIZE,
#endif
#ifdef ZMQ_THREAD_AFFINITY_CPU_ADD
  thread_affinity_cpu_add = ZMQ_THREAD_AFFINITY_CPU_ADD,
#endif
#ifdef ZMQ_THREAD_AFFINITY_CPU_REMOVE
  thread_affinity_cpu_remove = ZMQ_THREAD_AFFINITY_CPU_REMOVE,
#endif
#ifdef ZMQ_THREAD_NAME_PREFIX
  thread_name_prefix = ZMQ_THREAD_NAME_PREFIX,
#endif
#ifdef ZMQ_ZERO_COPY_RCV
  zero_copy_rcv = ZMQ_ZERO_COPY_RCV,
#endif
};

context::context()
    : ptr_{zmq_ctx_new(), [](void *ptr) noexcept {
             if (ptr != nullptr)
               zmq_ctx_term(ptr);
           }} {
  if (!ptr_)
    throw error();
}

void context::set(context_opt name, int value) const {
  int rc = zmq_ctx_set(ptr_.get(), static_cast<int>(name), value);
  if (rc != 0)
    throw error();
}
int context::get(context_opt name) const {
  int val = zmq_ctx_get(ptr_.get(), static_cast<int>(name));
  if (val < 0)
    throw error();
  return val;
}

context::operator void *() const noexcept { return ptr_.get(); }
context::operator bool() const noexcept { return static_cast<bool>(ptr_); }

enum struct socket_type : int {
#ifdef ZMQ_PAIR
  pair = ZMQ_PAIR,
#endif
#ifdef ZMQ_PUB
  pub = ZMQ_PUB,
#endif
#ifdef ZMQ_SUB
  sub = ZMQ_SUB,
#endif
#ifdef ZMQ_REQ
  req = ZMQ_REQ,
#endif
#ifdef ZMQ_REP
  rep = ZMQ_REP,
#endif
#ifdef ZMQ_DEALER
  dealer = ZMQ_DEALER,
#endif
#ifdef ZMQ_ROUTER
  router = ZMQ_ROUTER,
#endif
#ifdef ZMQ_PULL
  pull = ZMQ_PULL,
#endif
#ifdef ZMQ_PUSH
  push = ZMQ_PUSH,
#endif
#ifdef ZMQ_XPUB
  xpub = ZMQ_XPUB,
#endif
#ifdef ZMQ_XSUB
  xsub = ZMQ_XSUB,
#endif
#ifdef ZMQ_STREAM
  stream = ZMQ_STREAM,
#endif
#ifdef ZMQ_SERVER
  server = ZMQ_SERVER,
#endif
#ifdef ZMQ_CLIENT
  client = ZMQ_CLIENT,
#endif
#ifdef ZMQ_RADIO
  radio = ZMQ_RADIO,
#endif
#ifdef ZMQ_DISH
  dish = ZMQ_DISH,
#endif
#ifdef ZMQ_GATHER
  gather = ZMQ_GATHER,
#endif
#ifdef ZMQ_SCATTER
  scatter = ZMQ_SCATTER,
#endif
#ifdef ZMQ_DGRAM
  dgram = ZMQ_DGRAM,
#endif
};

enum struct socket_opt : int {
#ifdef ZMQ_AFFINITY
  affinity = ZMQ_AFFINITY,
#endif
#ifdef ZMQ_ROUTING_ID
  routing_id = ZMQ_ROUTING_ID,
#endif
#ifdef ZMQ_SUBSCRIBE
  subscribe = ZMQ_SUBSCRIBE,
#endif
#ifdef ZMQ_UNSUBSCRIBE
  unsubscribe = ZMQ_UNSUBSCRIBE,
#endif
#ifdef ZMQ_RATE
  rate = ZMQ_RATE,
#endif
#ifdef ZMQ_RECOVERY_IVL
  recovery_ivl = ZMQ_RECOVERY_IVL,
#endif
#ifdef ZMQ_SNDBUF
  sndbuf = ZMQ_SNDBUF,
#endif
#ifdef ZMQ_RCVBUF
  rcvbuf = ZMQ_RCVBUF,
#endif
#ifdef ZMQ_RCVMORE
  rcvmore = ZMQ_RCVMORE,
#endif
#ifdef ZMQ_FD
  fd = ZMQ_FD,
#endif
#ifdef ZMQ_EVENTS
  events = ZMQ_EVENTS,
#endif
#ifdef ZMQ_TYPE
  type = ZMQ_TYPE,
#endif
#ifdef ZMQ_LINGER
  linger = ZMQ_LINGER,
#endif
#ifdef ZMQ_RECONNECT_IVL
  reconnect_ivl = ZMQ_RECONNECT_IVL,
#endif
#ifdef ZMQ_BACKLOG
  backlog = ZMQ_BACKLOG,
#endif
#ifdef ZMQ_RECONNECT_IVL_MAX
  reconnect_ivl_max = ZMQ_RECONNECT_IVL_MAX,
#endif
#ifdef ZMQ_MAXMSGSIZE
  maxmsgsize = ZMQ_MAXMSGSIZE,
#endif
#ifdef ZMQ_SNDHWM
  sndhwm = ZMQ_SNDHWM,
#endif
#ifdef ZMQ_RCVHWM
  rcvhwm = ZMQ_RCVHWM,
#endif
#ifdef ZMQ_MULTICAST_HOPS
  multicast_hops = ZMQ_MULTICAST_HOPS,
#endif
#ifdef ZMQ_RCVTIMEO
  rcvtimeo = ZMQ_RCVTIMEO,
#endif
#ifdef ZMQ_SNDTIMEO
  sndtimeo = ZMQ_SNDTIMEO,
#endif
#ifdef ZMQ_LAST_ENDPOINT
  last_endpoint = ZMQ_LAST_ENDPOINT,
#endif
#ifdef ZMQ_ROUTER_MANDATORY
  router_mandatory = ZMQ_ROUTER_MANDATORY,
#endif
#ifdef ZMQ_TCP_KEEPALIVE
  tcp_keepalive = ZMQ_TCP_KEEPALIVE,
#endif
#ifdef ZMQ_TCP_KEEPALIVE_CNT
  tcp_keepalive_cnt = ZMQ_TCP_KEEPALIVE_CNT,
#endif
#ifdef ZMQ_TCP_KEEPALIVE_IDLE
  tcp_keepalive_idle = ZMQ_TCP_KEEPALIVE_IDLE,
#endif
#ifdef ZMQ_TCP_KEEPALIVE_INTVL
  tcp_keepalive_intvl = ZMQ_TCP_KEEPALIVE_INTVL,
#endif
#ifdef ZMQ_IMMEDIATE
  immediate = ZMQ_IMMEDIATE,
#endif
#ifdef ZMQ_XPUB_VERBOSE
  xpub_verbose = ZMQ_XPUB_VERBOSE,
#endif
#ifdef ZMQ_ROUTER_RAW
  router_raw = ZMQ_ROUTER_RAW,
#endif
#ifdef ZMQ_IPV6
  ipv6 = ZMQ_IPV6,
#endif
#ifdef ZMQ_MECHANISM
  mechanism = ZMQ_MECHANISM,
#endif
#ifdef ZMQ_PLAIN_SERVER
  plain_server = ZMQ_PLAIN_SERVER,
#endif
#ifdef ZMQ_PLAIN_USERNAME
  plain_username = ZMQ_PLAIN_USERNAME,
#endif
#ifdef ZMQ_PLAIN_PASSWORD
  plain_password = ZMQ_PLAIN_PASSWORD,
#endif
#ifdef ZMQ_CURVE_SERVER
  curve_server = ZMQ_CURVE_SERVER,
#endif
#ifdef ZMQ_CURVE_PUBLICKEY
  curve_publickey = ZMQ_CURVE_PUBLICKEY,
#endif
#ifdef ZMQ_CURVE_SECRETKEY
  curve_secretkey = ZMQ_CURVE_SECRETKEY,
#endif
#ifdef ZMQ_CURVE_SERVERKEY
  curve_serverkey = ZMQ_CURVE_SERVERKEY,
#endif
#ifdef ZMQ_PROBE_ROUTER
  probe_router = ZMQ_PROBE_ROUTER,
#endif
#ifdef ZMQ_REQ_CORRELATE
  req_correlate = ZMQ_REQ_CORRELATE,
#endif
#ifdef ZMQ_REQ_RELAXED
  req_relaxed = ZMQ_REQ_RELAXED,
#endif
#ifdef ZMQ_CONFLATE
  conflate = ZMQ_CONFLATE,
#endif
#ifdef ZMQ_ZAP_DOMAIN
  zap_domain = ZMQ_ZAP_DOMAIN,
#endif
#ifdef ZMQ_ROUTER_HANDOVER
  router_handover = ZMQ_ROUTER_HANDOVER,
#endif
#ifdef ZMQ_TOS
  tos = ZMQ_TOS,
#endif
#ifdef ZMQ_CONNECT_ROUTING_ID
  connect_routing_id = ZMQ_CONNECT_ROUTING_ID,
#endif
#ifdef ZMQ_GSSAPI_SERVER
  gssapi_server = ZMQ_GSSAPI_SERVER,
#endif
#ifdef ZMQ_GSSAPI_PRINCIPAL
  gssapi_principal = ZMQ_GSSAPI_PRINCIPAL,
#endif
#ifdef ZMQ_GSSAPI_SERVICE_PRINCIPAL
  gssapi_service_principal = ZMQ_GSSAPI_SERVICE_PRINCIPAL,
#endif
#ifdef ZMQ_GSSAPI_PLAINTEXT
  gssapi_plaintext = ZMQ_GSSAPI_PLAINTEXT,
#endif
#ifdef ZMQ_HANDSHAKE_IVL
  handshake_ivl = ZMQ_HANDSHAKE_IVL,
#endif
#ifdef ZMQ_SOCKS_PROXY
  socks_proxy = ZMQ_SOCKS_PROXY,
#endif
#ifdef ZMQ_XPUB_NODROP
  xpub_nodrop = ZMQ_XPUB_NODROP,
#endif
#ifdef ZMQ_BLOCKY
  blocky = ZMQ_BLOCKY,
#endif
#ifdef ZMQ_XPUB_MANUAL
  xpub_manual = ZMQ_XPUB_MANUAL,
#endif
#ifdef ZMQ_XPUB_WELCOME_MSG
  xpub_welcome_msg = ZMQ_XPUB_WELCOME_MSG,
#endif
#ifdef ZMQ_STREAM_NOTIFY
  stream_notify = ZMQ_STREAM_NOTIFY,
#endif
#ifdef ZMQ_INVERT_MATCHING
  invert_matching = ZMQ_INVERT_MATCHING,
#endif
#ifdef ZMQ_HEARTBEAT_IVL
  heartbeat_ivl = ZMQ_HEARTBEAT_IVL,
#endif
#ifdef ZMQ_HEARTBEAT_TTL
  heartbeat_ttl = ZMQ_HEARTBEAT_TTL,
#endif
#ifdef ZMQ_HEARTBEAT_TIMEOUT
  heartbeat_timeout = ZMQ_HEARTBEAT_TIMEOUT,
#endif
#ifdef ZMQ_XPUB_VERBOSER
  xpub_verboser = ZMQ_XPUB_VERBOSER,
#endif
#ifdef ZMQ_CONNECT_TIMEOUT
  connect_timeout = ZMQ_CONNECT_TIMEOUT,
#endif
#ifdef ZMQ_TCP_MAXRT
  tcp_maxrt = ZMQ_TCP_MAXRT,
#endif
#ifdef ZMQ_THREAD_SAFE
  thread_safe = ZMQ_THREAD_SAFE,
#endif
#ifdef ZMQ_MULTICAST_MAXTPDU
  multicast_maxtpdu = ZMQ_MULTICAST_MAXTPDU,
#endif
#ifdef ZMQ_VMCI_BUFFER_SIZE
  vmci_buffer_size = ZMQ_VMCI_BUFFER_SIZE,
#endif
#ifdef ZMQ_VMCI_BUFFER_MIN_SIZE
  vmci_buffer_min_size = ZMQ_VMCI_BUFFER_MIN_SIZE,
#endif
#ifdef ZMQ_VMCI_BUFFER_MAX_SIZE
  vmci_buffer_max_size = ZMQ_VMCI_BUFFER_MAX_SIZE,
#endif
#ifdef ZMQ_VMCI_CONNECT_TIMEOUT
  vmci_connect_timeout = ZMQ_VMCI_CONNECT_TIMEOUT,
#endif
#ifdef ZMQ_USE_FD
  use_fd = ZMQ_USE_FD,
#endif
#ifdef ZMQ_GSSAPI_PRINCIPAL_NAMETYPE
  gssapi_principal_nametype = ZMQ_GSSAPI_PRINCIPAL_NAMETYPE,
#endif
#ifdef ZMQ_GSSAPI_SERVICE_PRINCIPAL_NAMETYPE
  gssapi_service_principal_nametype = ZMQ_GSSAPI_SERVICE_PRINCIPAL_NAMETYPE,
#endif
#ifdef ZMQ_BINDTODEVICE
  bindtodevice = ZMQ_BINDTODEVICE,
#endif
#ifdef ZMQ_ZAP_ENFORCE_DOMAIN
  zap_enforce_domain = ZMQ_ZAP_ENFORCE_DOMAIN,
#endif
#ifdef ZMQ_LOOPBACK_FASTPATH
  loopback_fastpath = ZMQ_LOOPBACK_FASTPATH,
#endif
#ifdef ZMQ_METADATA
  metadata = ZMQ_METADATA,
#endif
};

enum struct monitor_event : int {
#ifdef ZMQ_EVENT_CONNECTED
  event_connected = ZMQ_EVENT_CONNECTED,
#endif
#ifdef ZMQ_EVENT_CONNECT_DELAYED
  event_connect_delayed = ZMQ_EVENT_CONNECT_DELAYED,
#endif
#ifdef ZMQ_EVENT_CONNECT_RETRIED
  event_connect_retried = ZMQ_EVENT_CONNECT_RETRIED,
#endif
#ifdef ZMQ_EVENT_LISTENING
  event_listening = ZMQ_EVENT_LISTENING,
#endif
#ifdef ZMQ_EVENT_BIND_FAILED
  event_bind_failed = ZMQ_EVENT_BIND_FAILED,
#endif
#ifdef ZMQ_EVENT_ACCEPTED
  event_accepted = ZMQ_EVENT_ACCEPTED,
#endif
#ifdef ZMQ_EVENT_ACCEPT_FAILED
  event_accept_failed = ZMQ_EVENT_ACCEPT_FAILED,
#endif
#ifdef ZMQ_EVENT_CLOSED
  event_closed = ZMQ_EVENT_CLOSED,
#endif
#ifdef ZMQ_EVENT_CLOSE_FAILED
  event_close_failed = ZMQ_EVENT_CLOSE_FAILED,
#endif
#ifdef ZMQ_EVENT_DISCONNECTED
  event_disconnected = ZMQ_EVENT_DISCONNECTED,
#endif
#ifdef ZMQ_EVENT_MONITOR_STOPPED
  event_monitor_stopped = ZMQ_EVENT_MONITOR_STOPPED,
#endif
#ifdef ZMQ_EVENT_ALL
  event_all = ZMQ_EVENT_ALL,
#endif
};

socket::socket(const context &ctx, socket_type type)
    : ptr_{zmq_socket(static_cast<void *>(ctx), static_cast<int>(type)),
           [](void *ptr) noexcept { zmq_close(ptr); }} {
  if (!ptr_)
    throw error();
}

template <class T> void socket::set(socket_opt name, const T &value) const {
  int rc =
      zmq_setsockopt(ptr_.get(), static_cast<int>(name), &value, sizeof(T));
  if (rc != 0)
    throw error();
}
template <class T> void socket::get(socket_opt name, T &value) const {
  std::size_t sz = sizeof(T);
  int rc = zmq_getsockopt(ptr_.get(), static_cast<int>(name), &value, &sz);
  if (rc != 0)
    throw error();
}

void socket::bind(const char *endpoint) const {
  int rc = zmq_bind(ptr_.get(), endpoint);
  if (rc != 0)
    throw error();
}
void socket::connect(const char *endpoint) const {
  int rc = zmq_connect(ptr_.get(), endpoint);
  if (rc != 0)
    throw error();
}
void socket::unbind(const char *endpoint) const {
  int rc = zmq_unbind(ptr_.get(), endpoint);
  if (rc != 0)
    throw error();
}
void socket::disconnect(const char *endpoint) const {
  int rc = zmq_disconnect(ptr_.get(), endpoint);
  if (rc != 0)
    throw error();
}

int socket::send(message msg, bool wait) const {
  int bytes = msg.send(*this, wait);
  return bytes;
}
template <class T>
int socket::send(std::size_t len, const T *data, int flags) const {
  int bytes =
      zmq_send_const(static_cast<void *>(*this), data, len * sizeof(T), flags);
  if (bytes < 0)
    throw error();
  return bytes;
}

message socket::receive(bool wait) const {
  message msg;
  msg.receive(*this, wait);
  return msg;
}
template <class T>
int socket::receive(std::size_t len, T *data, int flags) const {
  int bytes =
      zmq_recv(static_cast<void *>(*this), data, len * sizeof(T), flags);
  if (bytes < 0)
    throw error();
  return bytes;
}

socket socket::monitor(const context &ctx, const char *endpoint,
                       monitor_event event) const {
  return monitor(ctx, endpoint, static_cast<int>(event));
}
socket socket::monitor(const context &ctx, const char *endpoint,
                       int events) const {
  int rc = zmq_socket_monitor(static_cast<void *>(*this), endpoint, events);
  if (rc < 0)
    throw error();
  socket listener(ctx, socket_type::pair);
  listener.connect(endpoint);
  return listener;
}

socket::operator void *() const noexcept { return ptr_.get(); }

message::part::part() noexcept { zmq_msg_init(&msg_); }
message::part::part(std::size_t size) {
  int rc = zmq_msg_init_size(&msg_, size);
  if (rc != 0)
    throw error();
}
template <class T>
message::part::part(
    const T &value,
    typename std::enable_if<std::is_arithmetic<T>::value>::type *)
    : part(sizeof(T)) {
  std::memcpy(zmq_msg_data(&msg_), &value, sizeof(T));
}
template <class T>
message::part::part(std::size_t len, const T *data) : part(len * sizeof(T)) {
  std::memcpy(zmq_msg_data(&msg_), data, len * sizeof(T));
}
template <class T>
message::part::part(std::size_t len, T *data, zmq_free_fn *ffn, void *hint) {
  int rc = zmq_msg_init_data(&msg_, data, len * sizeof(T), ffn, hint);
  if (rc != 0)
    throw error();
}
template <class Iter> message::part::part(Iter begin, Iter end) {
  using size_type = typename std::iterator_traits<Iter>::difference_type;
  using value_type = typename std::iterator_traits<Iter>::value_type;
  size_type sz = std::distance(begin, end) * sizeof(value_type);
  int rc = zmq_msg_init_size(&msg_, sz);
  if (rc != 0)
    throw error();
  value_type *dest = static_cast<value_type *>(zmq_msg_data(&msg_));
  std::copy(begin, end, dest);
}

message::part::part(const part &rhs) noexcept : part() {
  zmq_msg_copy(&msg_, const_cast<zmq_msg_t *>(&rhs.msg_));
}
message::part &message::part::operator=(const part &rhs) noexcept {
  clear();
  zmq_msg_copy(&msg_, const_cast<zmq_msg_t *>(&rhs.msg_));
  return *this;
}
message::part::part(part &&rhs) noexcept : part() {
  zmq_msg_move(&msg_, &rhs.msg_);
}
message::part &message::part::operator=(part &&rhs) noexcept {
  clear();
  zmq_msg_move(&msg_, &rhs.msg_);
  return *this;
}

message::part::~part() { zmq_msg_close(&msg_); }

std::size_t message::part::size() const noexcept { return zmq_msg_size(&msg_); }
message::part::operator void *() noexcept { return zmq_msg_data(&msg_); }
void *message::part::data() noexcept { return static_cast<void *>(*this); }
bool message::part::more() const noexcept { return zmq_msg_more(&msg_) == 1; }

int message::part::send(const socket &s, int flags) {
  int bytes = zmq_msg_send(&msg_, static_cast<void *>(s), flags);
  if (bytes < 0)
    throw error();
  return bytes;
}
int message::part::receive(const socket &s, int flags) {
  clear();
  int bytes = zmq_msg_recv(&msg_, static_cast<void *>(s), flags);
  if (bytes < 0)
    throw error();
  return bytes;
}
template <class OutputIt>
OutputIt message::part::read(OutputIt begin, OutputIt end) noexcept {
  using value_type = typename std::iterator_traits<OutputIt>::value_type;
  const std::size_t iterlen = std::distance(begin, end);
  const std::size_t datalen = size() / sizeof(value_type);
  value_type *source = static_cast<value_type *>(static_cast<void *>(*this));
  return std::copy(source, source + std::min(iterlen, datalen), begin);
}
void message::part::clear() noexcept {
  zmq_msg_close(&msg_);
  zmq_msg_init(&msg_);
}

message::message() noexcept(noexcept(std::vector<part>())) = default;
message::message(std::vector<part> parts) noexcept : parts_(std::move(parts)) {}

std::size_t message::addpart() {
  parts_.push_back(part());
  return parts_.back().size();
}
std::size_t message::addpart(std::size_t size) {
  parts_.push_back(part(size));
  return parts_.back().size();
}
template <class T> std::size_t message::addpart(const T &value) {
  part msg = value;
  parts_.push_back(std::move(msg));
  return parts_.back().size();
}
template <class T>
std::size_t message::addpart(std::size_t len, const T *data) {
  parts_.emplace_back(len, data);
  return parts_.back().size();
}
template <class T>
std::size_t message::addpart(std::size_t len, T *data, zmq_free_fn *ffn,
                             void *hint) {
  parts_.emplace_back(len, data, ffn, hint);
  return parts_.back().size();
}
template <class Iter> std::size_t message::addpart(Iter begin, Iter end) {
  parts_.emplace_back(begin, end);
  return parts_.back().size();
}

void *message::data(std::size_t pid) noexcept {
  return pid < numparts() ? static_cast<void *>(parts_[pid]) : nullptr;
}

std::size_t message::numparts() const noexcept { return parts_.size(); }
std::size_t message::size() const noexcept {
  std::size_t size_ = 0;
  for (const auto &part : parts_)
    size_ += part.size();
  return size_;
}
std::size_t message::size(std::size_t pid) const noexcept {
  return pid < numparts() ? parts_[pid].size() : 0;
}

message::part &message::operator[](std::size_t pid) { return parts_[pid]; }
const message::part &message::operator[](std::size_t pid) const {
  return parts_[pid];
}

void message::pop_back() { parts_.pop_back(); }

int message::send(const socket &s, bool wait) {
  int flags = wait ? 0 : ZMQ_DONTWAIT;
  int bytes, total{0};
  const std::size_t len = parts_.size();
  for (std::size_t idx = 0; idx < len; idx++) {
    bytes = parts_[idx].send(s, (idx < len - 1) ? flags | ZMQ_SNDMORE : flags);
    total += bytes;
  }
  return total;
}
int message::receive(const socket &s, bool wait) {
  clear();
  int flags = wait ? 0 : ZMQ_DONTWAIT;
  int bytes, total{0};
  part msgpart;

  bytes = msgpart.receive(s, flags);
  total = bytes;
  while (msgpart.more()) {
    parts_.push_back(std::move(msgpart));
    bytes = msgpart.receive(s, flags);
    total += bytes;
  }
  parts_.push_back(std::move(msgpart));
  return total;
}
template <class OutputIt>
OutputIt message::read(std::size_t pid, OutputIt begin, OutputIt end) noexcept {
  return pid < numparts() ? parts_[pid].read(begin, end) : begin;
}
void message::clear() noexcept { parts_.clear(); }

enum struct poll_event : short {
#ifdef ZMQ_POLLIN
  pollin = ZMQ_POLLIN,
#endif
#ifdef ZMQ_POLLOUT
  pollout = ZMQ_POLLOUT,
#endif
#ifdef ZMQ_POLLERR
  poller = ZMQ_POLLERR,
#endif
#ifdef ZMQ_POLLPRI
  pollpri = ZMQ_POLLPRI,
#endif
};

poller::item::item(const zmq_pollitem_t item) noexcept : item_{item} {}

bool poller::item::belongsto(const socket &s) const noexcept {
  return item_.socket == static_cast<void *>(s);
}
bool poller::item::isready() const noexcept {
  return item_.events & item_.revents;
}

poller::poller() noexcept(noexcept(std::vector<zmq_pollitem_t>())) = default;

void poller::additem(const socket &s, poll_event pe) {
  additem(s, static_cast<short>(pe));
}
void poller::additem(const socket &s, short events) {
  zmq_pollitem_t item;
  item.socket = static_cast<void *>(s);
  item.fd = 0;
  item.events = events;
  item.revents = 0;
  items_.push_back(item);
}

std::size_t poller::size() const noexcept { return items_.size(); }
poller::item poller::operator[](std::size_t idx) const {
  return item{items_[idx]};
}

int poller::poll(long timeout) {
  int rc = zmq_poll(&items_[0], items_.size(), timeout);
  if (rc < 0)
    throw error();
  return rc;
}

void poller::clear() noexcept { items_.clear(); }
} // namespace zmq
} // namespace communicator
} // namespace polo

#endif
