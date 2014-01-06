Gem::Specification.new do |spec|
  spec.name = 'bartlby'
  spec.version = '0.1'
  spec.summary = 'Summary'
  spec.description = 'Description'
  spec.email = 'helmut@januschka.com'
  spec.homepage = 'http://www.bartlby.org'
  spec.author = 'Helmut Januschka'
  spec.bindir = 'bin'
  spec.executable = 'exec.rb'
  spec.files = Dir['lib/**/*.rb'] + Dir['bin/*'] + Dir['ext/**/*.so'] + Dir['ext/**/*.dll']
  spec.platform = Gem::Platform::RUBY
  spec.require_paths = [ 'lib', 'ext' ]
  spec.extensions = Dir['ext/**/extconf.rb']
end
