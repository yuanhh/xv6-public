#pragma once

struct padded_length {
  volatile u64 v_ __mpalign__;;
  __padout__;
};

#if defined (XV6_KERNEL)
struct uwq;

struct uwq_worker {
  long wait();

  uwq* uwq;
  bool running;
  proc *proc;
  struct spinlock lock;
  struct condvar cv;
};

struct uwq : public referenced, public rcu_freed {
  static uwq* alloc(vmap* vmap, filetable *ftable);
  bool  haswork();
  bool  trywork();
  void* buffer();

  void  setuentry(uptr uentry);
  // XXX(sbw) should be part of struct worker
  void  tryexit(uwq_worker *w);

  virtual void do_gc(void) { delete this; }

protected:
  virtual void onzero() const;

private:
  uwq(vmap* vmap, filetable* ftable, padded_length *len);
  ~uwq();
  uwq& operator=(const uwq&);
  uwq(const uwq& x);
  proc* allocworker();
  void  finishworkers();
  void  finish();
  NEW_DELETE_OPS(uwq);

  struct spinlock lock_;
  vmap* vmap_;
  filetable* ftable_;
  padded_length* len_;
  uptr uentry_;
  uptr ustack_;
  std::atomic<u64> uref_;

  uwq_worker worker_[NCPU];
};

int             uwq_trywork(void);
#endif
