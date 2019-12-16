# Linking C++ software stacks with SystemVerilog using DPI

## How to build

```bash
module load waf
mkdir my_project && cd my_project/

# first build
waf setup --project flange
srun -p compile -c8 -- singularity exec --app visionary-dls /containers/stable/latest waf configure install --test-execnone

# run (software) tests
singularity exec --app visionary-dls /containers/stable/latest waf install --test-execall
```

## Documentation

Doxygen-generated code documentation is deployed [here](https://jenkins.bioai.eu/job/bld_nightly-flange/Documentation_20_28flange_29/) nightly by Jenkins.