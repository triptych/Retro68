/* Verify that indirect inlining can also remove references of the functions it
   discovers calls for, even when nodes being inlined are virtual IPA-CP
   clones.  */
/* { dg-do compile } */
/* { dg-options "-O3 -fno-early-inlining -fdump-ipa-cp-details -fdump-ipa-inline -fdump-tree-optimized"  } */


int global;

void __attribute__ ((noinline, noclone, used))
stuff (int i)
{
  global = i;
}

int __attribute__ ((noinline,noclone)) get_input(void)
{
  return 1;
}

static void
hooray_1 ()
{
  stuff (1);
}

static inline void
hip2_1 (void (*g)())
{
  int i;
  g ();
  /* Some stuff to make the function bigger so that hip1_1 gets inlined
     fiorst. */
  for (i = 0; i < get_input (); i++)
    {
      stuff (2);
      stuff (2+2);
    }
}

static inline void
hip1_1 (void (*g)())
{
  hip2_1 (g);
}

static void
hooray_2 ()
{
  stuff (1);
}

static inline void
hip2_2 (void (*g)())
{
  g ();
}

static inline void
hip1_2 (void (*g)())
{
  int i;

  hip2_2 (g);

  /* Some stuff to make the function bigger so that hip2_2 gets inlined
     fiorst. */
  for (i = 0; i < get_input (); i++)
    {
      stuff (2);
      stuff (2+2);
    }
}

int
main (int argc, int *argv[])
{
  int i;

  for (i = 0; i < get_input (); i++)
    {
      hip1_1 (hooray_1);
      hip1_2 (hooray_2);
    }
  return 0;
}

/* { dg-final { scan-ipa-dump-times "removing its cloning-created reference" 2 "cp"  } } */
/* { dg-final { scan-ipa-dump "ipa-prop: Removed a reference"  "inline"  } } */
/* { dg-final { scan-ipa-dump-times "ipa-prop: Removing cloning-created reference" 2 "inline"  } } */
/* { dg-final { scan-tree-dump-not "hooray"  "optimized"  } } */
/* { dg-final { cleanup-ipa-dump "cp" } } */
/* { dg-final { cleanup-ipa-dump "inline" } } */
/* { dg-final { cleanup-tree-dump "optimized" } } */