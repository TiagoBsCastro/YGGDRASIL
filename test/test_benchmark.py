import pytest
import numpy as np

import pyfof


@pytest.mark.parametrize("use_brute", [False, True])
def test_two_gaussian_blobs(benchmark, use_brute):
    points_per_blob = 10000
    data = np.vstack([
        np.random.normal(-1, 0.2, (points_per_blob, 2)),
        np.random.normal(1, 0.2, (points_per_blob, 2)),
    ])

    groups = benchmark(lambda: pyfof.friends_of_friends(data, 0.4, use_brute))
    assert len(groups) == 2
    assert len(groups[0]) == points_per_blob
    assert len(groups[1]) == points_per_blob
