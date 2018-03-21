// <copyright company="Soup">
//        Copyright (c) Soup.  All rights reserved.
// </copyright>

using Soup.Api;
using Soup.TestUtils;
using System;
using System.Threading.Tasks;
using Xunit;

namespace Soup.Client.UnitTests
{
	public class InstallCommandTests : IDisposable
	{
		LocalUserConfig _config;

		public InstallCommandTests()
		{
			_config = new LocalUserConfig();
			Singleton<ISoupApi>.Instance = new MockSoupApi();
		}

		public void Dispose()
		{
			_config = null;
		}

		[Fact]
		public void NameIsCorrect()
		{
			var uut = new InstallCommand();
			Assert.Equal("install", uut.Name);
		}

		[Fact]
		public async Task CallWithZeroArgsShouldInstallAll()
		{
			var uut = new InstallCommand();

			var args = new string[] { };
			await uut.InvokeAsync(args, _config);
		}


	}
}