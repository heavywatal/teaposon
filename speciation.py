#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
"""
import wtl.options as wopt

program = 'tek'


def iter_values():
    crossing_axes = wopt.OrderedDict()
    crossing_axes['xi'] = ['10e-4']  # '5e-4', '1e-4']
    crossing_axes['lower'] = ['6', '9', '12', '300']
    crossing_axes['upper'] = ['18', '24', '30', '300']
    for d in wopt.product(crossing_axes):
        if (d['lower'] == '300'):
            if (d['upper'] != '300'):
                continue
        else:
            if (d['upper'] == '300'):
                continue
        yield wopt.OrderedDict(**d)


def iter_values_spec():
    crossing_axes = wopt.OrderedDict()
    crossing_axes['xi'] = ['1e-4', '5e-4']
    crossing_axes['spec'] = ['1e-4', '1e-3']
    for d in wopt.product(crossing_axes):
        yield wopt.OrderedDict(**d)


def iter_args(rest, concurrency, repeat, skip):
    const = [program, '-j{}'.format(concurrency), '-r7'] + rest
    suffix = '_{}'.format(wopt.now())
    for i, v in enumerate(wopt.cycle(iter_values(), repeat)):
        if i < skip:
            continue
        args = wopt.make_args(v)
        label = wopt.join(args) + suffix + '_{:02}'.format(i)
        yield const + args + ['--outdir=' + label]


def main():
    parser = wopt.ArgumentParser()
    parser.add_argument('--skip', type=int, default=0)
    parser.add_argument('-o', '--outdir', default='.stdout')
    parser.add_argument('-r', '--repeat', type=int, default=1)
    (args, rest) = parser.parse_known_args()
    print("cpu_count(): {}".format(wopt.cpu_count()))
    print('{} jobs * {} threads/job'.format(args.jobs, args.parallel))

    wopt.map_async(iter_args(rest, args.parallel, args.repeat, args.skip),
                   args.jobs, args.dry_run, outdir=args.outdir)
    print('End of ' + __file__)


if __name__ == '__main__':
    main()
