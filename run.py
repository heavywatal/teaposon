import wtl.options as wopt


def te2fig1():
    crossing_axes = wopt.OrderedDict()
    crossing_axes['n'] = ['1000']
    crossing_axes['xi'] = ['10e-4', '5e-4', '1e-4']
    crossing_axes['g'] = ['50000']
    crossing_axes['i'] = ['100']
    for d in wopt.product(crossing_axes):
        yield wopt.OrderedDict(**d)


def te2fig2():
    crossing_axes = wopt.OrderedDict()
    crossing_axes['n'] = ['1000']
    crossing_axes['xi'] = ['10e-4']
    crossing_axes['g'] = ['50000']
    crossing_axes['i'] = ['100']
    for d in wopt.product(crossing_axes):
        yield wopt.OrderedDict(**d)


def te2fig4():
    return [{
        'n': '1000',
        'xi': '10e-4',
        'g': '6000',
        'H': '4000',
        'i': '20',
    }]


def te2fig5():
    crossing_axes = wopt.OrderedDict()
    crossing_axes['n'] = ['1000']
    crossing_axes['xi'] = ['10e-4']
    crossing_axes['coexist'] = ['2']
    crossing_axes['lower'] = ['6', '9']
    crossing_axes['upper'] = ['18', '24', '30']
    crossing_axes['g'] = ['50000']
    crossing_axes['i'] = ['100']
    for d in wopt.product(crossing_axes):
        yield wopt.OrderedDict(**d)


def te2fig6():
    crossing_axes = wopt.OrderedDict()
    crossing_axes['r'] = ['1']
    crossing_axes['n'] = ['1000']
    crossing_axes['xi'] = ['10e-4']
    crossing_axes['coexist'] = ['2', '5', '8']
    crossing_axes['lower'] = ['6', '9']
    crossing_axes['upper'] = ['18', '24', '30']
    crossing_axes['g'] = ['50000']
    crossing_axes['i'] = ['100']
    for d in wopt.product(crossing_axes):
        yield wopt.OrderedDict(**d)


def te1fig2s():
    parallel_axes = wopt.OrderedDict()
    parallel_axes['alpha'] = ['0.70', '0.75', '0.80', '0.85']
    parallel_axes['beta'] = [6, 12, 24, 48]
    crossing_axes = wopt.OrderedDict()
    crossing_axes['xi'] = ['1e-5', '1e-4', '1e-3']
    crossing_axes['lambda'] = ['1e-4', '1e-3']
    crossing_axes['nu'] = ['0', '1e-6', '1e-4']
    for di in wopt.parallel(parallel_axes):
        for dj in wopt.product(crossing_axes):
            yield wopt.OrderedDict(**di, **dj)


def iter_args(arg_maker, rest, concurrency, repeat, skip):
    const = ['tek2', '-j{}'.format(concurrency)] + rest
    now = wopt.now()
    for i, v in enumerate(wopt.cycle(arg_maker(), repeat)):
        if i < skip:
            continue
        args = wopt.make_args(v)
        label = '_'.join([wopt.join(args), now, format(i, '03d')])
        yield const + args + ['--outdir=' + label]


def main():
    arg_makers = {k: v for k, v in globals().items()
                  if callable(v) and k not in ('main', 'iter_args')}
    parser = wopt.ArgumentParser()
    parser.add_argument('function', choices=arg_makers)
    (args, rest) = parser.parse_known_args()
    print("cpu_count(): {}".format(wopt.cpu_count()))
    print('{} jobs * {} threads/job'.format(args.jobs, args.parallel))

    fun = arg_makers[args.function]
    it = iter_args(fun, rest, args.parallel, args.repeat, args.skip)
    wopt.map_async(it, args.jobs, args.dry_run, outdir=args.outdir)
    print('End of ' + __file__)


if __name__ == '__main__':
    main()
