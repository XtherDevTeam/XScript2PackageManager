# A package manager for XScript 2

## Usage

You need to initialize xpm by `xpm init` at first run.

```
xpm switch-mirror host=http[s]://hostname

xpm install [global] name=PackageName ver=PackageVersion
xpm uninstall [global] name=PackageName
xpm create-project name=ProjectName

xpm build-project
xpm run-project
xpm pack-build-dir

xpm run-package package=PackageName exec=ExecFileName

xpm init
```

## Known issues

- xpm won't remove unnecessary dependency packages during uninstall process