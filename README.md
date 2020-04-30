# Linking C++ software stacks with SystemVerilog using DPI

## How to build

```bash
module load waf
mkdir my_project && cd my_project/

# first build
waf setup --project flange
srun -p compile -c8 -- singularity exec --app dls-core /containers/stable/latest waf configure install --test-execnone

# run (software) tests
singularity exec --app dls-core /containers/stable/latest waf install --test-execall
```

## Documentation

Doxygen-generated code documentation is deployed [here](https://jenkins.bioai.eu/job/bld_nightly-flange/Documentation_20_28flange_29/) nightly by Jenkins.

## License
```
Linking C++ software stacks with SystemVerilog using DPI ('flange')
Copyright (C) 2018-2020 Electronic Vision(s) Group
                        Kirchhoff-Institute for Physics
                        Ruprecht-Karls-Universität Heidelberg
                        69120 Heidelberg
                        Germany

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
USA
```
